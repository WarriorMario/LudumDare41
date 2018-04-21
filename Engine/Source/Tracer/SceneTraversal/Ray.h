#pragma once
#include "Typedefs.h"
#include "Math/Vec3.h"
#include "Math/MathUtilities.h"

class Ray
{
public:
  Ray() // ray towards positive z
    :
    o(Vec3(kZeroF, kZeroF, kZeroF)),
    d(Vec3(kZeroF, kZeroF, kOneF)),
    t(INFINITY)
  {}
  Ray(const Vec3& O, const Vec3& D, Float T = INFINITY)
    :
    o(O),
    d(D),
    t(T)
  {}

public:
  Vec3 o, d;
  Float t;
};