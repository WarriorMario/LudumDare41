#pragma once

namespace Tmpl8
{
  class Mesh;
}
class DynamicBVH
{
  const static size_t primPerNode = 3;
  class Node
  {
  public:
    void Subdivide(DynamicBVH& bvh, size_t depth);
    bool Traverse(DynamicBVH& bvh, Mesh* mesh, Result& result, Ray& ray);
    bool TraverseShadow(DynamicBVH& bvh, Ray& ray);

    bool Intersect(Ray& ray);

  public:
    union
    {
      struct
      {
        vec3 bmin;
        int firstLeft;
      };
      __m128 bmin4;
    };
    union
    {
      struct
      {
        vec3 bmax;
        int count;
      };
      __m128 bmax4;
    };
    //AABB aabb;
    //unsigned int firstLeft;
    //unsigned int count;
  };

public:
  DynamicBVH(FILE* file);
  DynamicBVH(const Mesh* mesh);
  AABB CalculateAABB(unsigned int firstLeft, unsigned int count);
  bool Traverse(Mesh* mesh, Result& result, Ray& ray);
  bool TraverseShadow(Ray& ray);
  AABB GetAABB();
  void Serialize(FILE* file);
  void Deserialize(FILE* file);
  size_t m_numNodes;
  size_t m_numTris;
  size_t m_poolPtr;
  Triangle* m_tris;
  Node* m_nodes;
  // Debugging
  size_t m_maxDepth;
};
