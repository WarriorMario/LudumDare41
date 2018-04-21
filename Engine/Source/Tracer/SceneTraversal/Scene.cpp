#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddTriangle(const Triangle & tri)
{
  tris.push_back(tri);
}

void Scene::Optimize()
{
}

bool Scene::Intersect(Ray & ray, Intersection & hit)
{
  bool intersected = false;
  for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end(); ++it)
  {
    intersected |= it->Intersect(ray, hit);
  }
  return intersected;
}

bool Scene::IntersectShadow(Ray & ray)
{
  for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end(); ++it)
  {
    if (it->IntersectShadow(ray))
    {
      return true;
    }
  }
  return false;
}
