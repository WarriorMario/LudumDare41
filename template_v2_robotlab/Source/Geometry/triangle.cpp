#include "precomp.h"

Triangle::Triangle()
{
}
vec3 Triangle::Center()
{
  return ( pos[0] + pos[1] + pos[2] ) / 3.0f;
}
bool Triangle::Intersect(Ray& ray, Mesh* mesh, Result& result)
{ 
  // get the points
  const vec3& p1 = pos[0];
  const vec3& p2 = pos[1];
  const vec3& p3 = pos[2];

  // calc data needed to compute barycentric coordinates
  vec3 e1 = p2 - p1;
  vec3 e2 = p3 - p1;
  vec3 s1 = cross(ray.dir, e2);
  float divisor = dot(e1, s1);
  if (divisor == 0.0f)
  {
    return false;
  }
  float invDivisor = 1.0f / divisor;

  // compute first barycentric coordinate
  vec3 s = ray.pos - p1;
  float b1 = dot(s, s1) * invDivisor;
  if (b1 < 0.0f || b1 > 1.0f)
  {
    return false;
  }

  // compute second barycentric coordinate
  vec3 s2 = cross(s, e1);
  float b2 = dot(ray.dir, s2) * invDivisor;
  if (b2 < 0.0f || b1 + b2 > 1.0f)
  {
    return false;
  }

  // compute t to hit
  float t = dot(e2, s2) * invDivisor;
  if (t < 0.0f || t > ray.len)
  {
    return false;
  }

  // a hit is now certain

  float b0 = 1.0f - b1 - b2;

  ray.len = t;
  result.bary_coords = vec3(b0, b1, b2);
  result.tri = this;
  result.mesh = mesh;

  return true;
}

bool Triangle::ShadowIntersect(Ray& ray)
{ 
  // get the points
  const vec3& p1 = pos[0];
  const vec3& p2 = pos[1];
  const vec3& p3 = pos[2];

  // calc data needed to compute barycentric coordinates
  vec3 e1 = p2 - p1;
  vec3 e2 = p3 - p1;
  vec3 s1 = cross(ray.dir, e2);
  float divisor = dot(e1, s1);
  if (divisor == 0.0f)
  {
    return false;
  }
  float invDivisor = 1.0f / divisor;

  // compute first barycentric coordinate
  vec3 s = ray.pos - p1;
  float b1 = dot(s, s1) * invDivisor;
  if (b1 < 0.0f || b1 > 1.0f)
  {
    return false;
  }

  // compute second barycentric coordinate
  vec3 s2 = cross(s, e1);
  float b2 = dot(ray.dir, s2) * invDivisor;
  if (b2 < 0.0f || b1 + b2 > 1.0f)
  {
    return false;
  }

  // compute t to hit
  float t = dot(e2, s2) * invDivisor;
  if (t < 0.0f || t > ray.len)
  {
    return false;
  }

  // a hit is now certain
  return true;
}

vec3 Triangle::GetPosAtB(const vec3& b)
{
  return (b.x * pos[0] + b.y * pos[1] + b.z* pos[2]);
}

vec3 Triangle::GetNormalAtB(const vec3& b)
{
  return (b.x * norm[0] + b.y * norm[1] + b.z* norm[2]).normalized();
}

vec2 Triangle::GetUVAtB(const vec3& b)
{
  vec2 uv = (b.x * uv[0] + b.y * uv[1] + b.z* uv[2]);
  return uv;
}