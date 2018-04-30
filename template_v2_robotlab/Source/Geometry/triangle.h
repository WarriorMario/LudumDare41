#pragma once
namespace Tmpl8
{
  class Mesh;
}

class Result;
class Ray;
class Triangle
{
public:
  Triangle();

  bool Intersect( Ray& ray, Mesh* mesh, Result& result );
  bool ShadowIntersect( Ray& ray );

  // get the position, normal and UV coordinates from the barycentric coordinates
  vec3 GetPosAtB( const vec3& b );
  vec3 GetNormalAtB( const vec3& b );
  vec2 GetUVAtB( const vec3& b );

  vec3 Center();

  vec3 pos[3];
  vec3 norm[3];
  vec2 uv[3];
};