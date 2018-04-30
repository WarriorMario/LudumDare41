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
  Triangle* tri;
  vec3 bary_coords;
};