#pragma once
#include "Triangle.h"

class Intersection
{
public:
  Intersection() {}
  Intersection( Triangle* triangle, const Vec3& bary_coords)
    :
    tri(triangle),
    b(bary_coords)
  {}
public:
  Triangle* tri; // the triangle we intersected
  Vec3 b; // the barycentric coordinates
};