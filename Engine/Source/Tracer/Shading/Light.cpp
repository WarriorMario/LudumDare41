#include "Light.h"
#include "SceneTraversal/Ray.h"

Ray Light::GetRayFromPoint(Vec3 p)
{
  Vec3 dir = pos_ - p;
  Float t = dir.Magnitude();
  return Ray(p, dir / t, t);
}

FColor Light::GetLightAtPoint(Vec3 p)
{
  // TODO (Floris) actual lighting calculation with fallof and stuff
  return col_;
}
