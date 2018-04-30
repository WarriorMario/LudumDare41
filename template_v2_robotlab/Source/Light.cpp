#include "precomp.h"

Ray Light::GetRayFromPoint(vec3 p)
{
  vec3 dir = pos - p;
  float t = dir.length();
  float invT = 1.0f / t;
  dir *= invT;
  return Ray(p + 1.f * dir,
    dir, t);
}

FColor Light::GetLightAtPoint(vec3 p)
{
  // TODO (Floris) actual lighting calculation with fallof and stuff
  return col;
}
