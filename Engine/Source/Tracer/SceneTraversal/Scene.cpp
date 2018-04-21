#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddMesh(const Mesh & mesh)
{
  meshes.push_back(mesh);
}


void Scene::Optimize()
{
}

bool Scene::Intersect(Ray & ray, Intersection & hit)
{
  bool intersected = false;
  for (std::vector<Mesh>::iterator it = meshes.begin(); it != meshes.end(); ++it)
  {
    intersected |= it->Intersect(ray, hit);
  }
  return intersected;
}

bool Scene::IntersectShadow(Ray & ray)
{
  for (std::vector<Mesh>::iterator it = meshes.begin(); it != meshes.end(); ++it)
  {
    if (it->IntersectShadow(ray))
    {
      return true;
    }
  }
  return false;
}
