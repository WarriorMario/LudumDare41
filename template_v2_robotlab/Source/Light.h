#pragma once

class Ray;
class Light
{
public:
  Light()
    :
    pos(0.0f),
    col(0.0f)
  {}

  Light(vec3 pos, FColor col)
    :
    pos(pos),
    col(col)
  {}

  Ray GetRayFromPoint(vec3 p);
  FColor GetLightAtPoint(vec3 p);

public:
  vec3 pos;
  FColor col;
};