#pragma once
#include "Math/Vec3.h"

class Ray;

/*
* AABB
*
*   struct that contains the Axis Aligned Bounding Box of an object
*/
class AABB
{
public:
  AABB();
  AABB(Vec3 a_p);
  AABB(Vec3 a_p1, Vec3 a_p2);
  AABB(const AABB& rhs);

  AABB Union(const Vec3& a_p) const;
  AABB Union(const AABB& a_aabb2) const;

  bool Overlaps(const AABB& a_aabb) const;
  bool Inside(const Vec3& a_p) const;

  void Expand(Float delta);

  Float SurfaceArea() const;
  Float Volume() const;

  int32 MaxExtent() const;

  Vec3 Lerp(Float tx, Float ty, Float tz) const;

  Vec3 Offset(const Vec3& v) const;

  void BoundingSphere(Float* r, Vec3* p) const;

  bool Intersect(const Ray& ray, const Vec3& invDir, const int32 dirIsNeg[3]) const;

  inline const bool Contains(const AABB& rhs) const
  {
    return
      min.x < rhs.min.x && min.y < rhs.min.y && min.z < rhs.min.z &&
      max.x > rhs.max.x && max.y > rhs.max.y && max.z > rhs.max.z;
  }

  inline const bool operator==(const AABB& rhs) const
  {
    AABB temp = *this;
    temp.Expand(EPSILON); // apply an error margin
    bool fits_in = temp.Contains(rhs);
    temp.Expand(EPSILON * -kTwoF);
    return fits_in && rhs.Contains(temp);
  }

  inline const bool operator!=(const AABB& rhs) const
  {
    return !(*this == rhs);
  }

  inline const Vec3& operator[](const int32 i) const
  {
    return (&min)[i];
  }
  inline Vec3& operator[](const int32 i)
  {
    return (&min)[i];
  }

  inline Vec3 Corner(int32 i) const
  {
    return Vec3((*this)[i & 1].x,
      (*this)[i & 2 ? 1 : 0].y,
      (*this)[i & 4 ? 1 : 0].z);
  }

public:
  Vec3 min, max;
};