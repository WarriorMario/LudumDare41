#pragma once
#include "Mesh.h"
#include "Shading/Light.h"


class Ray;
class Intersection;

class Scene
{
public:
  Scene();
  ~Scene();

  void AddMesh(const Mesh& tri);

  void Optimize();

  bool Intersect(Ray& ray, Intersection& hit);
  bool IntersectShadow(Ray& ray);

  // quick crappy workaround for an obvious design flaw
  std::vector<Light>& GetLights()
  {
    return lights;
  }

private:
  std::vector<Mesh> meshes;
  std::vector<Light> lights;
};

