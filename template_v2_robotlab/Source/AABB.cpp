#include "precomp.h"
#undef min;
#undef max;

AABB::AABB()
{
  min = vec3( INFINITY, INFINITY, INFINITY );
  max = vec3( -INFINITY, -INFINITY, -INFINITY );
}

AABB::AABB( vec3 a_p )
{
  min = a_p;
  max = a_p;
}

AABB::AABB( vec3 a_p1, vec3 a_p2 )
{
  min = vec3( MIN( a_p1.x, a_p2.x ), MIN( a_p1.y, a_p2.y ), MIN( a_p1.z, a_p2.z ) );
  max = vec3( MAX( a_p1.x, a_p2.x ), MAX( a_p1.y, a_p2.y ), MAX( a_p1.z, a_p2.z ) );
}
AABB::AABB( const AABB & rhs )
  :
  min( rhs.min ),
  max( rhs.max )
{
}

AABB AABB::Union( const vec3 & a_p ) const
{
  AABB res;
  res.min = vec3(
    MIN( min.x, a_p.x ),
    MIN( min.y, a_p.y ),
    MIN( min.z, a_p.z ) );
  res.max = vec3(
    MAX( max.x, a_p.x ),
    MAX( max.y, a_p.y ),
    MAX( max.z, a_p.z ) );
  return res;
}

AABB AABB::Union( const AABB & a_aabb ) const
{
  AABB res;
  res.min = vec3( MIN(
    min.x, a_aabb.min.x ),
    MIN( min.y, a_aabb.min.y ),
    MIN( min.z, a_aabb.min.z ) );

  res.max = vec3(
    MAX( max.x, a_aabb.max.x ),
    MAX( max.y, a_aabb.max.y ),
    MAX( max.z, a_aabb.max.z ) );
  return res;
}

bool AABB::Overlaps( const AABB & a_aabb ) const
{
  bool x = ( max.x >= a_aabb.min.x ) && ( min.x <= a_aabb.max.x );
  bool y = ( max.y >= a_aabb.min.y ) && ( min.y <= a_aabb.max.y );
  bool z = ( max.z >= a_aabb.min.z ) && ( min.z <= a_aabb.max.z );
  return x && y && z;
}

bool AABB::Inside( const vec3 & a_p ) const
{
  bool x = ( max.x >= a_p.x ) && ( min.x <= a_p.x );
  bool y = ( max.y >= a_p.y ) && ( min.y <= a_p.y );
  bool z = ( max.z >= a_p.z ) && ( min.z <= a_p.z );
  return x && y && z;
}

void AABB::Expand( float delta )
{
  vec3 expander = vec3( delta, delta, delta );
  min -= expander;
  max += expander;
}

float AABB::SurfaceArea() const
{
  vec3 d = max - min;
  return 2.0f * ( d.x * d.y + d.x * d.z + d.y * d.z );
}

float AABB::Volume() const
{
  vec3 d = max - min;
  return d.x * d.y * d.z;
}

int AABB::MaxExtent() const
{
  vec3 d = max - min;
  if ( d.x > d.y && d.x > d.z )
  {
    return 0;
  }
  else if ( d.y > d.z )
  {
    return 1;
  }
  else
  {
    return 2;
  }
  return 0;
}

vec3 AABB::Center() const
{
  return ( min + max ) * 0.5f;
}

//vec3 AABB::Lerp( float tx, float ty, float tz ) const
//{
//  return vec3(
//    Lerp( min.x, max.x, tx ),
//    Lerp( min.y, max.y, ty ),
//    Lerp( min.z, max.z, tz ) );
//}

vec3 AABB::Offset( const vec3 & v ) const
{
  return vec3( ( v.x - min.x ) / ( max.x - min.x ),
    ( v.y - min.y ) / ( max.y - min.y ),
    ( v.z - min.z ) / ( max.z - min.z ) );
}

void AABB::BoundingSphere( float * r, vec3 * p ) const
{
  *p = 0.5f * min + 0.5f * max;
  *r = ( *p - max ).length();
}

bool AABB::Intersect( const Ray & ray, const vec3 & invDir, const int dirIsNeg[3] ) const
{
  const AABB& aabb = *this;

  /// to enable this, add a * operator to vec3 that multiplies lhs.x with rhs.x etcetera
  //vec3 minPoint = aabb.min;
  //vec3 maxPoint = aabb.max;
  //vec3 minV = (minPoint - ray.o) * invDir;
  //vec3 maxV = (maxPoint - ray.o) * invDir;
  ////Generates new direction, so we have to recaclutate min and max point
  //float tmin = Max(Max(Min(minV.x, maxV.x), Min(minV.y, maxV.y)), Min(minV.z, maxV.z));
  //float tmax = Min(Min(Max(minV.x, maxV.x), Max(minV.y, maxV.y)), Max(minV.z, maxV.z));
  //return (tmax >= tmin);

  // find x and y t max and min
  float tMin = ( aabb[dirIsNeg[0]].x - ray.pos.x ) * invDir.x;
  float tMax = ( aabb[1 - dirIsNeg[0]].x - ray.pos.x ) * invDir.x;
  float tyMin = ( aabb[dirIsNeg[1]].y - ray.pos.y ) * invDir.y;
  float tyMax = ( aabb[1 - dirIsNeg[1]].y - ray.pos.y ) * invDir.y;

  // check if inside the box
  if ( ( tMin > tyMax ) || ( tyMin > tMax ) )
  {
    return false;
  }
  // find the extremes
  if ( tyMin > tMin )
  {
    tMin = tyMin;
  }
  if ( tyMax < tMax )
  {
    tMax = tyMax;
  }

  // repeat for z axis
  float tzMin = ( aabb[dirIsNeg[2]].z - ray.pos.z ) * invDir.z;
  float tzMax = ( aabb[1 - dirIsNeg[2]].z - ray.pos.z ) * invDir.z;

  if ( ( tMin > tzMax ) || ( tzMin > tMax ) )
  {
    return false;
  }
  if ( tzMin > tMin )
  {
    tMin = tzMin;
  }
  if ( tzMax < tMax )
  {
    tMax = tzMax;
  }

  // return true if the ray intersects
  if ( tMin > ray.len )
  {
    return false;
  }
  if ( tMin > 0 )
  {
    return true;
  }

  // if tMin is smaller then zero, we need to check if we're in the AABB: 
  //    if not, the box is behind us 

  return Inside( ray.pos );
}

bool AABB::Intersect( const Ray& ray ) const
{
  vec3 rDir = vec3( 1.f/ray.dir.x ,1.f/ray.dir.y , 1.f/ray.dir.z );
  vec3 tMin = ( min - ray.pos ) * rDir, tMax = ( max - ray.pos ) * rDir;
  vec3 t1 = Min( tMin, tMax ), t2 = Max( tMin, tMax );
  float tNear = MAX( MAX( t1.x, t1.y ), t1.z );
  float tFar = MIN( MIN( t2.x, t2.y ), t2.z );
  return ( ( tFar >= tNear ) && ( tNear < ray.len ) && ( tFar > 0 ) );
}
