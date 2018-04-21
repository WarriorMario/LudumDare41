#include "Triangle.h"
#include "Ray.h"
#include "Intersection.h"

Triangle::Triangle()
  :
  col_(FColor(kZeroF))
{
  for (int32 i = 0; i < 3; ++i)
  {
    p_[i] = Vec3(kZeroF);
    n_[i] = Vec3::GetForward();
    uv_[i] = Vec2(kZeroF);
  }
}

Triangle::Triangle(const Vec3* p, const Vec3* n, const Vec2* uv, const FColor& col)
  :
  col_(col)
{
  for (int32 i = 0; i < 3; ++i)
  {
    p_[i] = p[i];
    n_[i] = n[i];
    uv_[i] = uv[i];
  }
}

bool Triangle::Intersect(Ray& ray, Intersection& hit)
{
  // get the points
  const Vec3& p1 = p_[0];
  const Vec3& p2 = p_[1];
  const Vec3& p3 = p_[2];

  // calc data needed to compute barycentric coordinates
  Vec3 e1 = p2 - p1;
  Vec3 e2 = p3 - p1;
  Vec3 s1 = Cross(ray.d, e2);
  Float divisor = Dot(s1, e1);
  if (divisor == kZeroF)
  {
    return false;
  }
  Float invDivisor = 1.0f / divisor;

  // compute first barycentric coordinate
  Vec3 s = ray.o - p1;
  Float b1 = Dot(s, s1) * invDivisor;
  if (b1 < kZeroF || b1 > kOneF)
  {
    return false;
  }

  // compute second barycentric coordinate
  Vec3 s2 = Cross(s, e1);
  Float b2 = Dot(ray.d, s2) * invDivisor;
  if (b2 < kZeroF || b1 + b2 > kOneF)
  {
    return false;
  }

  // compute t to hit
  Float t = Dot(e2, s2) * invDivisor;
  if (t < kZeroF || t > ray.t)
  {
    return false;
  }

  // a hit is now certain

  Float b0 = kOneF - b1 - b2;

  ray.t = t;
  hit = Intersection(this, Vec3(b0, b1, b2));

  return true;
}

bool Triangle::IntersectShadow(Ray& ray)
{
  // get the points
  const Vec3& p1 = p_[0];
  const Vec3& p2 = p_[1];
  const Vec3& p3 = p_[2];

  // calc data needed to compute barycentric coordinates
  Vec3 e1 = p2 - p1;
  Vec3 e2 = p3 - p1;
  Vec3 s1 = Cross(ray.d, e2);
  Float divisor = Dot(s1, e1);
  if (divisor == kZeroF)
  {
    return false;
  }
  Float invDivisor = 1.0f / divisor;

  // compute first barycentric coordinate
  Vec3 s = ray.o - p1;
  Float b1 = Dot(s, s1) * invDivisor;
  if (b1 < kZeroF || b1 > kOneF)
  {
    return false;
  }

  // compute second barycentric coordinate
  Vec3 s2 = Cross(s, e1);
  Float b2 = Dot(ray.d, s2) * invDivisor;
  if (b2 < kZeroF || b1 + b2 > kOneF)
  {
    return false;
  }

  // compute t to hit
  Float t = Dot(e2, s2) * invDivisor;
  if (t < kZeroF || t > ray.t)
  {
    return false;
  }

  // a hit is now certain
  return true;
}

Vec3 Triangle::GetPosAtB(const Vec3& b)
{
  return (b.x * p_[0] + b.y * p_[1] + b.z* p_[2]);
}

Vec3 Triangle::GetNormalAtB(const Vec3& b)
{
 return (b.x * n_[0] + b.y * n_[1] + b.z* n_[2]).Normalized();
}

Vec2 Triangle::GetUVAtB(const Vec3& b)
{
  return (b.x * uv_[0] + b.y * uv_[1] + b.z* uv_[2]);
}
