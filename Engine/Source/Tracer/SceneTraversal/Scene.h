#pragma once
#include "Triangle.h"
#include "Shading/Light.h"

#include <vector>

class Ray;
class Intersection;

class Scene
{
public:
  Scene();
  ~Scene();

  void AddTriangle(const Triangle& tri);

  void Optimize();

  bool Intersect(Ray& ray, Intersection& hit);
  bool IntersectShadow(Ray& ray);

  // quick crappy workaround for an obvious design flaw
  std::vector<Light>& GetLights()
  {
    return lights;
  }

private:
  std::vector<Triangle> tris;
  std::vector<Light> lights;
};

