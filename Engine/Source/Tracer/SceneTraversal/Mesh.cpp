#include "Mesh.h"

Mesh::Mesh()
{
}

void Mesh::SetTransform(const Transform & new_transform)
{
  tranny = new_transform;
}

void Mesh::AddTriangle(const Triangle & tri)
{
  tris.push_back(tri);
}

bool Mesh::Intersect(Ray & ray, Intersection & hit)
{
  Ray r;
  tranny.GetInverse().TransformRay(ray, &r);

  bool intersected = false;
  for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end(); ++it)
  {
    intersected |= it->Intersect(r, hit);
  }
  return intersected;
}

bool Mesh::IntersectShadow(Ray & ray)
{
  Ray r;
  tranny.GetInverse().TransformRay(ray, &r);

  for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end(); ++it)
  {
    if (it->IntersectShadow(r))
    {
      return true;
    }
  }
  return false;
}
