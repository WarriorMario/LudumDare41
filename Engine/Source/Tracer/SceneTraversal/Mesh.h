#pragma once
#include "Triangle.h"
#include "Math/Transform.h"
#include <vector>

class Intersection;
class Ray;

class Mesh
{
public:
  Mesh();
  void SetTransform(const Transform& new_transform);
  void AddTriangle(const Triangle& tri);

  bool Intersect(Ray& ray, Intersection& hit);
  bool IntersectShadow(Ray& ray);

private:
  std::vector<Triangle> tris;
  Transform tranny;
};

