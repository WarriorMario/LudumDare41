#pragma once
class Ray
{
public:
  Ray(){};
  // constructor / destructor
  Ray( vec3 origin, vec3 direction, float distance ) : pos( origin ), dir( direction ), len( distance ) {}
  // data members
  vec3 pos, dir;
  float len;
};
constexpr int packetWidth = 64;
constexpr int packetHeight = 64;
constexpr int packetSize = packetWidth * packetHeight;
struct RayPacket
{
  Ray rays[packetSize];
};