#pragma once
#include "Typedefs.h"
#include "FColor.h"
#include "Math/Vec3.h"

class Ray;

class Light
{
public:
  Light(Vec3 pos, FColor col)
    :
    pos_(pos),
    col_(col)
  {}

  Ray GetRayFromPoint(Vec3 p);
  FColor GetLightAtPoint(Vec3 p);
  

private:
  Vec3 pos_;
  FColor col_;
};