#include "precomp.h"
#undef min;
#undef max;

DynamicBVH::DynamicBVH( FILE * file )
{
  Deserialize( file );
}

DynamicBVH::DynamicBVH( const Mesh* mesh )
{
  std::vector<Triangle> triangles;
  triangles.reserve( 10000 );
  for ( int i = 0; i < mesh->tris; ++i )
  {
    Triangle tri;
    for ( int v = 0; v < 3; ++v )
    {
      tri.pos[v] = mesh->pos[mesh->tri[i * 3 + v]];
      tri.norm[v] = mesh->norm[mesh->tri[i * 3 + v]];
      tri.uv[v] = mesh->uv[mesh->tri[i * 3 + v]];
    }
    triangles.push_back( tri );
  }

  m_tris = ( Triangle* )_aligned_malloc( triangles.size() * sizeof( Triangle ), 128 );
  for ( int i = 0; i < triangles.size(); ++i )
  {
    m_tris[i] = triangles[i];
  }
  const size_t nodeSize = sizeof( Node );
  //static_assert( nodeSize == 32, "Node is not 32 bytes" );

  m_numTris = triangles.size();
  m_numNodes = m_numTris * 2 + 1;
  m_nodes = ( Node* )_aligned_malloc( m_numNodes * sizeof( Node ), 128 );
  // Resize the buffer back to regain some of the memory that isn't used
  Node* root = &m_nodes[0];
  m_poolPtr = 1;
  m_maxDepth = 0;
  root->firstLeft = 0;
  root->count = ( int )m_numTris;
  root->Subdivide( *this, 0 ); // construct the first node
  m_numNodes = m_poolPtr+1;
}

AABB DynamicBVH::CalculateAABB( unsigned int firstLeft, unsigned int count )
{
  Triangle& init = m_tris[firstLeft];
  AABB ret;
  ret.min = init.pos[0];
  ret.max = init.pos[0];
  for ( size_t i = firstLeft; i < firstLeft + count; ++i )
  {
    Triangle& t = m_tris[i];
    ret.min.x = ( MIN( MIN( t.pos[0].x, MIN( t.pos[1].x, t.pos[2].x ) ), ret.min.x ) );
    ret.min.y = ( MIN( MIN( t.pos[0].y, MIN( t.pos[1].y, t.pos[2].y ) ), ret.min.y ) );
    ret.min.z = ( MIN( MIN( t.pos[0].z, MIN( t.pos[1].z, t.pos[2].z ) ), ret.min.z ) );
    ret.max.x = ( MAX( MAX( t.pos[0].x, MAX( t.pos[1].x, t.pos[2].x ) ), ret.max.x ) );
    ret.max.y = ( MAX( MAX( t.pos[0].y, MAX( t.pos[1].y, t.pos[2].y ) ), ret.max.y ) );
    ret.max.z = ( MAX( MAX( t.pos[0].z, MAX( t.pos[1].z, t.pos[2].z ) ), ret.max.z ) );
  }
  return ret;
}

bool DynamicBVH::Traverse(Mesh* mesh, Result& result, Ray & ray)
{
  return m_nodes[0].Traverse( *this, mesh, result, ray );
}

bool DynamicBVH::TraverseShadow( Ray & ray )
{
  return m_nodes[0].TraverseShadow( *this, ray );
}

AABB DynamicBVH::GetAABB()
{
  //return m_nodes[0].aabb;
  return AABB(m_nodes[0].bmin, m_nodes[0].bmax);
}

void DynamicBVH::Serialize( FILE * file )
{
  fwrite( &m_numNodes, sizeof( m_numNodes ), 1, file );
  fwrite( &m_numTris, sizeof( m_numTris ), 1, file );
  fwrite( m_tris, sizeof( Triangle ), m_numTris, file );
  fwrite( m_nodes, sizeof( Node ), m_numNodes, file );
  fflush( file );
}

void DynamicBVH::Deserialize( FILE * file )
{
  fread( &m_numNodes, sizeof( m_numNodes ), 1, file );
  fread( &m_numTris, sizeof( m_numTris ), 1, file );
  m_nodes = ( Node* )_aligned_malloc( m_numNodes * sizeof( Node ), 64 );
  m_tris = (Triangle*)_aligned_malloc( m_numTris * sizeof( Triangle ), 64 );
  fread( m_tris, sizeof( Triangle ), m_numTris, file );
  fread( m_nodes, sizeof( Node ), m_numNodes, file );
}

void Sort(Triangle* triangles, int start, int count, int idx)
{
  int L = start;
  int R = (start + count) - 1;
  float center = triangles[(L + R) / 2].Center()[idx];

  while (L < R)
  {
    while (triangles[L].Center()[idx] < center)
    {
      L++;
    }
    while (triangles[R].Center()[idx] > center)
    {
      R--;
    }

    if (L <= R)
    {
      Triangle left = triangles[L];
      triangles[L] = triangles[R];
      triangles[R] = left;
      L++;
      R--;
    }
  }

  if (start < R)
  {
    Sort(triangles, start, R - start, idx);
  }
  if (L < ((start + count) - 1))
  {
    Sort(triangles, L, ((start + count) - 1) - L, idx);
  }
}

void CalcBestCost(DynamicBVH& bvh, int firstLeft, int count, int axis,
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
void DynamicBVH::Node::Subdivide( DynamicBVH & bvh, size_t depth )
{
  bvh.m_maxDepth = MAX( bvh.m_maxDepth, depth );
  // Calculate the new bounds
  //aabb = bvh.CalculateAABB(firstLeft, count);
  AABB aabb = bvh.CalculateAABB( firstLeft, count );
  bmin = aabb.min;
  bmax = aabb.max;

  // Quit when we reached the Minimum amount of primitives
  if ( count < primPerNode )
    return;
  // Construct the nodes
  ++bvh.m_poolPtr;
  Node* leftNode = &bvh.m_nodes[bvh.m_poolPtr];
  ++bvh.m_poolPtr;
  Node* rightNode = &bvh.m_nodes[bvh.m_poolPtr];

  // Store best data
  float bestCost = aabb.SurfaceArea()*count;
  size_t bestAxis = 2;
  size_t bestLeftCount, bestRightCount;

  // Initial cost
  float initialCost = bestCost;

  // sort on all axes, find the best one
  for (int i = 0; i < 3; ++i)
  {
    Sort(bvh.m_tris, firstLeft, count, i);
    CalcBestCost(bvh, firstLeft, count, i, bestCost,
      bestLeftCount, bestRightCount, bestAxis);
  }
  
  // Resort to the best axis. (it's currently sorted for z so it can be ignored)
  // Z is where we left so we don't have to sort it
  if ( bestAxis != 2 )
  {
    Sort( bvh.m_tris, firstLeft, count, bestAxis);
    bestCost = initialCost;
    CalcBestCost( bvh, firstLeft, count, bestAxis, initialCost,
       bestLeftCount, bestRightCount, bestAxis );
  }
  if ( initialCost == bestCost )// Splitting will not help us
  {
     // We won't be using these nodes now
     bvh.m_poolPtr -= 2;
     return;
  }
  leftNode->firstLeft = firstLeft;
  leftNode->count = ( int )bestLeftCount;
  rightNode->firstLeft = firstLeft + ( int )bestLeftCount;
  rightNode->count = ( int )bestRightCount;
  if ( bestLeftCount == 0 || bestRightCount == 0 )
  {
    bvh.m_poolPtr -= 2;
    return;
  }
  firstLeft = ( int )bvh.m_poolPtr - 1;// Save our left node
  count = 0; // We're not a leaf
  leftNode->Subdivide( bvh, depth + 1 );
  rightNode->Subdivide( bvh, depth + 1 );
}


bool DynamicBVH::Node::Traverse( DynamicBVH& bvh, Mesh* mesh, Result& result, Ray & ray )
{
  // check if the ray intersects this Node
  if ( Intersect( ray ) )
  {
    // check if you're a leaf node
    if ( count > 0 )
    {
      const int lastPrim = firstLeft + count;
      bool intersects = false;
      for ( int i = firstLeft; i < lastPrim; ++i )
      {
        intersects |= bvh.m_tris[i].Intersect( ray, mesh, result );
      }
      return intersects;
    }
    else // branch node, check children recursively
    {
      bool intersects = false;
      intersects |= bvh.m_nodes[firstLeft].Traverse( bvh, mesh, result, ray );
      intersects |= bvh.m_nodes[firstLeft+1].Traverse( bvh, mesh, result, ray );
      return intersects;
    }
  }
  // the ray didn't intersect the node, so no hits were found
  return false;
}

bool DynamicBVH::Node::Intersect(Ray & ray)
{
  __m128 t1 = _mm_mul_ps(_mm_sub_ps(bmin4, ray.O4), ray.rD4);
  __m128 t2 = _mm_mul_ps(_mm_sub_ps(bmax4, ray.O4), ray.rD4);
  __m128 vmax4 = _mm_max_ps(t1, t2), vmin4 = _mm_min_ps(t1, t2);
  float* vmax = (float*)&vmax4, *vmin = (float*)&vmin4;
  float tmax = MIN(vmax[0], MIN(vmax[1], vmax[2]));
  float tmin = MAX(vmin[0], MAX(vmin[1], vmin[2]));
  return tmax >= tmin && tmax >= 0;
  //return false;
}

bool DynamicBVH::Node::TraverseShadow( DynamicBVH & bvh, Ray & ray )
{
  // check if the ray intersects this Node
  if ( Intersect( ray ) )
  {
    // check if you're a leaf node
    if ( count > 0 )
    {
      const int lastPrim = firstLeft + count;
      for ( int i = firstLeft; i < lastPrim; ++i )
      {
        if ( bvh.m_tris[i].ShadowIntersect( ray))
        {
          return true;
        }
      }
    }
    else // branch node, check children recursively
    {
      if ( bvh.m_nodes[firstLeft].TraverseShadow( bvh, ray ) )
      {
        return true;
      }
      if ( bvh.m_nodes[firstLeft+1].TraverseShadow( bvh, ray ) )
      {
        return true;
      }
    }
  }
  // the ray didn't intersect the node, so no hits were found
  return false;
}
