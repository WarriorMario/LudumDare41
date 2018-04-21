#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Shading/FColor.h"

class Ray;
class Intersection;

class Triangle
{
public:
  Triangle();
  Triangle(const Vec3* p, const Vec3* n, const Vec2* uv, const FColor& col);

  bool Intersect(Ray& ray, Intersection& hit);
  bool IntersectShadow(Ray& ray);
  
  // get the position, normal and UV coordinates from the barycentric coordinates
  Vec3 GetPosAtB(const Vec3& b);
  Vec3 GetNormalAtB(const Vec3& b);
  Vec2 GetUVAtB(const Vec3& b);

private:
  Vec3 p_[3];
  Vec3 n_[3];
  Vec2 uv_[3];

  FColor col_;
};

