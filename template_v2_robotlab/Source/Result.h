#pragma once
namespace Tmpl8
{
  class Mesh;
}
class Triangle;
class Result
{
public:
  Result();
  Mesh* mesh;
  Material mat;
  Triangle* tri;
  vec3 baryCoords;
  vec3 norm;
};