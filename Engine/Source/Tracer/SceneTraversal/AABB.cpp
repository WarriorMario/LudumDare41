#include "AABB.h"
#include "Ray.h"
#include "Math/MathUtilities.h"

AABB::AABB()
{
  min = Vec3(INFINITY, INFINITY, INFINITY);
  max = Vec3(-INFINITY, -INFINITY, -INFINITY);
}

AABB::AABB(Vec3 a_p)
{
  min = a_p;
  max = a_p;
}

AABB::AABB(Vec3 a_p1, Vec3 a_p2)
{
  min = Vec3(Min(a_p1.x, a_p2.x), Min(a_p1.y, a_p2.y), Min(a_p1.z, a_p2.z));
  max = Vec3(Max(a_p1.x, a_p2.x), Max(a_p1.y, a_p2.y), Max(a_p1.z, a_p2.z));
}

AABB::AABB(const AABB & rhs)
  :
  min(rhs.min),
  max(rhs.max)
{
}

AABB AABB::Union(const Vec3 & a_p) const
{
  AABB res;
  res.min = Vec3(
    Min(min.x, a_p.x),
    Min(min.y, a_p.y),
    Min(min.z, a_p.z));
  res.max = Vec3(
    Max(max.x, a_p.x),
    Max(max.y, a_p.y),
    Max(max.z, a_p.z));
  return res;
}

AABB AABB::Union(const AABB & a_aabb) const
{
  AABB res;
  res.min = Vec3(Min(
    min.x, a_aabb.min.x),
    Min(min.y, a_aabb.min.y),
    Min(min.z, a_aabb.min.z));

  res.max = Vec3(
    Max(max.x, a_aabb.max.x),
    Max(max.y, a_aabb.max.y),
    Max(max.z, a_aabb.max.z));
  return res;
}

bool AABB::Overlaps(const AABB & a_aabb) const
{
  bool x = (max.x >= a_aabb.min.x) && (min.x <= a_aabb.max.x);
  bool y = (max.y >= a_aabb.min.y) && (min.y <= a_aabb.max.y);
  bool z = (max.z >= a_aabb.min.z) && (min.z <= a_aabb.max.z);
  return x && y && z;
}

bool AABB::Inside(const Vec3 & a_p) const
{
  bool x = (max.x >= a_p.x) && (min.x <= a_p.x);
  bool y = (max.y >= a_p.y) && (min.y <= a_p.y);
  bool z = (max.z >= a_p.z) && (min.z <= a_p.z);
  return x && y && z;
}

void AABB::Expand(Float delta)
{
  Vec3 expander = Vec3(delta, delta, delta);
  min -= expander;
  max += expander;
}

Float AABB::SurfaceArea() const
{
  Vec3 d = max - min;
  return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

Float AABB::Volume() const
{
  Vec3 d = max - min;
  return d.x * d.y * d.z;
}

int32 AABB::MaxExtent() const
{
  Vec3 d = max - min;
  if (d.x > d.y && d.x > d.z)
  {
    return 0;
  }
  else if (d.y > d.z)
  {
    return 1;
  }
  else
  {
    return 2;
  }
  return 0;
}

Vec3 AABB::Lerp(Float tx, Float ty, Float tz) const
{
  return Vec3(
    ::Lerp(min.x, max.x, tx),
    ::Lerp(min.y, max.y, ty),
    ::Lerp(min.z, max.z, tz));
}

Vec3 AABB::Offset(const Vec3 & v) const
{
  return Vec3((v.x - min.x) / (max.x - min.x),
    (v.y - min.y) / (max.y - min.y),
    (v.z - min.z) / (max.z - min.z));
}

void AABB::BoundingSphere(Float * r, Vec3 * p) const
{
  *p = kHalfF * min + kHalfF * max;
  *r = (*p - max).Magnitude();
}

bool AABB::Intersect(const Ray & ray, const Vec3 & invDir, const int32 dirIsNeg[3]) const
{
  const AABB& aabb = *this;

  /// to enable this, add a * operator to Vec3 that multiplies lhs.x with rhs.x etcetera
  //Vec3 minPoint = aabb.min;
  //Vec3 maxPoint = aabb.max;
  //Vec3 minV = (minPoint - ray.o) * invDir;
  //Vec3 maxV = (maxPoint - ray.o) * invDir;
  ////Generates new direction, so we have to recaclutate min and max point
  //Float tmin = Max(Max(Min(minV.x, maxV.x), Min(minV.y, maxV.y)), Min(minV.z, maxV.z));
  //Float tmax = Min(Min(Max(minV.x, maxV.x), Max(minV.y, maxV.y)), Max(minV.z, maxV.z));
  //return (tmax >= tmin);

  // find x and y t max and min
  Float tMin = (aabb[dirIsNeg[0]].x - ray.o.x) * invDir.x;
  Float tMax = (aabb[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x;
  Float tyMin = (aabb[dirIsNeg[1]].y - ray.o.y) * invDir.y;
  Float tyMax = (aabb[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y;

  // check if inside the box
  if ((tMin > tyMax) || (tyMin > tMax))
  {
    return false;
  }
  // find the extremes
  if (tyMin > tMin)
  {
    tMin = tyMin;
  }
  if (tyMax < tMax)
  {
    tMax = tyMax;
  }

  // repeat for z axis
  Float tzMin = (aabb[dirIsNeg[2]].z - ray.o.z) * invDir.z;
  Float tzMax = (aabb[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z;

  if ((tMin > tzMax) || (tzMin > tMax))
  {
    return false;
  }
  if (tzMin > tMin)
  {
    tMin = tzMin;
  }
  if (tzMax < tMax)
  {
    tMax = tzMax;
  }

  // return true if the ray intersects
  if (tMin > ray.t)
  {
    return false;
  }
  if (tMin > 0)
  {
    return true;
  }

  // if tMin is smaller then zero, we need to check if we're in the AABB: 
  //    if not, the box is behind us 

  return Inside(ray.o);
}
