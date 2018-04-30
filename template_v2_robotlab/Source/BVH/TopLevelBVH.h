#pragma once
namespace Tmpl8
{
  class SGNode;
}
class TopLevelBVH
{
public:
  class Node
  {
  public:
    void Subdivide(TopLevelBVH& bvh, size_t depth);
    bool Traverse(TopLevelBVH& bvh, Ray& ray, Result& result);
    bool TraverseShadow(TopLevelBVH& bvh, Ray& ray);

  public:
    AABB aabb;
    unsigned int firstLeft;
    unsigned int count;
  };
  
public:
  TopLevelBVH(std::vector<DynamicBVH*>& bvhs, SGNode* scene_root);
  ~TopLevelBVH();

  AABB CalculateAABB(unsigned int firstLeft, unsigned int count);
  bool Traverse(Ray& ray, Result& result);
  bool TraverseShadow(Ray& ray);
  AABB GetAABB();

public:
  std::vector<mat4> transforms;
  std::vector<mat4> invertedTransforms;
  std::vector<AABB> transformedAABBs;
  std::vector<Mesh*> meshes;
  std::vector<DynamicBVH*>& bvhList;
  size_t m_numNodes;
  size_t m_poolPtr;
  Node* m_nodes;

  // Debugging
  size_t m_maxDepth;

  static constexpr size_t primPerNode = 3;
};

