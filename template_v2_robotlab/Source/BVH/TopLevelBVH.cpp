#include "precomp.h"
#undef min
#undef max

AABB TransformAABB(mat4& m, AABB& aabb)
{
  AABB res;
  for (int i = 0; i < 8; ++i)
  {
    res = res.Union((m * vec4(aabb.Corner(i), 1)).xyz);
  }
  return res;
}

TopLevelBVH::TopLevelBVH(std::vector<DynamicBVH*>& bvhs, SGNode* scene_root)
  :
  bvhList(bvhs)
{
  // get all meshes and their transforms
  for (SGNode* n : scene_root->child)
  {
    // Ignore first layer
    for (SGNode* node : n->child)
    {
      if (node->GetType() == SGNode::SG_TRANSFORM)
      {
        mat4 M = node->localTransform;
        for (uint i = 0; i < node->child.size(); i++)
        {
          mat4 m;
          meshes.push_back((Mesh*)node->child[i]);
          transforms.push_back(m);
          m.invert();
          invertedTransforms.push_back(m);
        }
      }
      else
      {
        meshes.push_back((Mesh*)node);
        transforms.push_back(mat4::identity());
        invertedTransforms.push_back(mat4::identity());
      }
    }
  }

  transformedAABBs.reserve(meshes.size());
  for (int i = 0; i < meshes.size(); ++i)
  {
    AABB aabb = bvhList[meshes[i]->bvhIndex]->GetAABB();
    transformedAABBs.push_back(TransformAABB(invertedTransforms[i], aabb));
  }

  const size_t nodeSize = sizeof(Node);
  //static_assert( nodeSize == 32, "Node is not 32 bytes" );

  m_numNodes = bvhList.size() * 2 + 1;
  m_nodes = (Node*)_aligned_malloc(m_numNodes * nodeSize, 128);

  // Resize the buffer back to regain some of the memory that isn't used
  Node* root = &m_nodes[0];
  m_poolPtr = 1;
  m_maxDepth = 0;

  root->firstLeft = 0;
  root->count = (int)bvhList.size();
  root->Subdivide(*this, 0); // construct the first node
  m_numNodes = m_poolPtr;
}

AABB TopLevelBVH::CalculateAABB(unsigned int firstLeft, unsigned int count)
{
  AABB ret = transformedAABBs[firstLeft];
  for (size_t i = firstLeft + 1; i < firstLeft + count; ++i)
  {
    ret = ret.Union(transformedAABBs[i]);
  }
  return ret;
}

bool TopLevelBVH::Traverse(Ray& ray, Result& result)
{
  return m_nodes[0].Traverse(*this, ray, result);
}

bool TopLevelBVH::TraverseShadow(Ray & ray)
{
  return m_nodes[0].TraverseShadow(*this, ray);
}

AABB TopLevelBVH::GetAABB()
{
  return m_nodes[0].aabb;
}

template <typename T>
void Swap(T& lhs, T& rhs)
{
  T temp = lhs;
  lhs = rhs;
  rhs = temp;
}

void Sort(mat4* transforms, mat4* invertedTransforms, AABB* transformedAABBs, 
  DynamicBVH** bvhs, int start, int count, int idx)
{
  int L = start;
  int R = (start + count) - 1;
  float center = transformedAABBs[(L + R) / 2].Center()[idx];

  while (L < R)
  {
    while (transformedAABBs[L].Center()[idx] < center)
    {
      L++;
    }
    while (transformedAABBs[R].Center()[idx] > center)
    {
      R--;
    }

    if (L <= R)
    {
      // sort the lists
      Swap(bvhs[L], bvhs[R]);
      Swap(transforms[L], transforms[R]);
      Swap(invertedTransforms[L], invertedTransforms[R]);
      Swap(transformedAABBs[L], transformedAABBs[R]);

      L++;
      R--;
    }
  }

  if (start < R)
  {
    Sort(transforms, invertedTransforms, transformedAABBs, bvhs, start, R - start, idx);
  }
  if (L < ((start + count) - 1))
  {
    Sort(transforms, invertedTransforms, transformedAABBs, bvhs, L, ((start + count) - 1) - L, idx);
  }
}

void CalcBestCost(TopLevelBVH& bvh, int firstLeft, int count, int axis,
  float& bestCost, size_t& bestLeftCount, size_t& bestRightCount,
  size_t& bestAxis)
{
  for (size_t i = firstLeft; i < firstLeft + count; ++i)
  {
    // Calculate AABBs
    float leftArea, rightArea;
    size_t leftCount = (i - firstLeft);
    if (leftCount == 0)// Do we need to handle this or should we just skip?
    {
      leftArea = 0.0f;
    }
    else
    {
      AABB left = bvh.CalculateAABB(firstLeft, (int)leftCount);
      leftArea = left.SurfaceArea();
    }

    size_t rightCount = count - leftCount;
    if (rightCount == 0)// Ditto with left
    {
      rightArea = 0.0f;
    }
    else
    {
      AABB right = bvh.CalculateAABB((int)i, (int)rightCount);
      rightArea = right.SurfaceArea();
    }
    // Calculate cost
    float cost = leftArea * leftCount + rightArea * rightCount;
    if (cost < bestCost)
    {
      bestCost = cost;
      bestLeftCount = leftCount;
      bestRightCount = rightCount;
      bestAxis = axis;
    }
  }
}

void TopLevelBVH::Node::Subdivide(TopLevelBVH & bvh, size_t depth)
{
  bvh.m_maxDepth = MAX(bvh.m_maxDepth, depth);
  // Calculate the new bounds
  aabb = bvh.CalculateAABB(firstLeft, count);
  // Quit when we reached the Minimum amount of primitives
  if (count < primPerNode)
    return;
  // Construct the nodes
  ++bvh.m_poolPtr;
  Node* leftNode = &bvh.m_nodes[bvh.m_poolPtr];
  ++bvh.m_poolPtr;
  Node* rightNode = &bvh.m_nodes[bvh.m_poolPtr];

  // Store best data
  float bestCost = aabb.SurfaceArea()*count;
  size_t bestAxis;
  size_t bestLeftCount, bestRightCount;
  // Initial cost
  float initialCost = bestCost;

  for (int i = 0; i < 3; ++i)
  {
    // Sort on axis i
    Sort(bvh.transforms.data(), bvh.invertedTransforms.data(),
      bvh.transformedAABBs.data(), bvh.bvhList.data(), firstLeft, count, i);

    // update the best cost
    CalcBestCost(bvh, firstLeft, count, i, bestCost,
      bestLeftCount, bestRightCount, bestAxis);
  }

  // sort to match the best axis (we're already sorted for z)
  if (bestAxis != 2)
  {
    Sort(bvh.transforms.data(), bvh.invertedTransforms.data(),
      bvh.transformedAABBs.data(), bvh.bvhList.data(), firstLeft, count, bestAxis);
    bestCost = initialCost;
    CalcBestCost( bvh, firstLeft, count, bestAxis, bestCost,
       bestLeftCount, bestRightCount, bestAxis );
  }
  if ( initialCost == bestCost )// Splitting will not help us
  {
     // We won't be using these nodes now
     bvh.m_poolPtr -= 2;
     return;
  }


  // split
  leftNode->firstLeft = firstLeft;
  leftNode->count = (int)bestLeftCount;
  rightNode->firstLeft = firstLeft + (int)bestLeftCount;
  rightNode->count = (int)bestRightCount;
  if (bestLeftCount == 0 || bestRightCount == 0)
  {
    bvh.m_poolPtr -= 2;
    return;
  }
  firstLeft = (int)bvh.m_poolPtr - 1;// Save our left node
  count = 0; // We're not a leaf
  leftNode->Subdivide(bvh, depth + 1);
  rightNode->Subdivide(bvh, depth + 1);
}


bool TopLevelBVH::Node::Traverse(TopLevelBVH& bvh, Ray& ray, Result& result)
{
  // check if the ray intersects this Node
  if (aabb.Intersect(ray))
  {
    // check if you're a leaf node
    if (count > 0)
    {
      const int lastPrim = firstLeft + count;
      bool intersects = false;
      for (int i = firstLeft; i < lastPrim; ++i)
      {
        // create a new ray in mesh space
        Ray r = Ray(
          (bvh.invertedTransforms[i] * vec4(ray.pos, 1)).xyz,
          (bvh.invertedTransforms[i] * vec4(ray.dir, 0)).xyz,
          INFINITY);

        // intersect the bvh in its own space
        if (bvh.bvhList[i]->Traverse(bvh.meshes[i], result, r))
        {
          intersects = true;
          ray.len = r.len;
        }
      }
      return intersects;
    }
    else // branch node, check children recursively
    {
      bool intersects = false;
      intersects |= bvh.m_nodes[firstLeft].Traverse(bvh, ray, result);
      intersects |= bvh.m_nodes[firstLeft + 1].Traverse(bvh, ray, result);
      return intersects;
    }
  }
  // the ray didn't intersect the node, so no hits were found
  return false;
}

bool TopLevelBVH::Node::TraverseShadow(TopLevelBVH & bvh, Ray & ray)
{
  // check if the ray intersects this Node
  if (aabb.Intersect(ray))
  {
    // check if you're a leaf node
    if (count > 0)
    {
      const int lastPrim = firstLeft + count;
      for (int i = firstLeft; i < lastPrim; ++i)
      {
        // create a new ray in mesh space
        Ray r = Ray(
          (bvh.invertedTransforms[i] * vec4(ray.pos, 1)).xyz,
          (bvh.invertedTransforms[i] * vec4(ray.dir, 0)).xyz,
          INFINITY);

        // intersect the bvh in its own space
        if (bvh.bvhList[i]->TraverseShadow(r))
        {
          return true;
        }
      }
    }
    else // branch node, check children recursively
    {
      if (bvh.m_nodes[firstLeft].TraverseShadow(bvh, ray))
      {
        return true;
      }
      if (bvh.m_nodes[firstLeft + 1].TraverseShadow(bvh, ray))
      {
        return true;
      }
    }
  }
  // the ray didn't intersect the node, so no hits were found
  return false;
}
