#pragma once
class Ray
{
public:
  Ray()
  {}
  Ray( vec3 origin, vec3 direction, float distance ) 
    : 
    pos( origin ), 
    dir( direction ),
    len( distance ),
    rD4(_mm_set_ps(0.0f, 1.0f / direction.z, 1.0f / direction.y, 1.0f / direction.x))
  {}

  void SetrD4()
  {
    rD4 = _mm_set_ps(0.0f, 1.0f / dir.z, 1.0f / dir.y, 1.0f / dir.x);
  }

public:
  // data members
  union
  {
    struct
    {
      vec3 pos;
      int dummy;
    };
    __m128 O4;
  };

  vec3 dir;
  float len;

  __m128 rD4;
};
constexpr int packetWidth = 64;
constexpr int packetHeight = 64;
constexpr int packetSize = packetWidth * packetHeight;
struct RayPacket
{
  Ray rays[packetSize];
};