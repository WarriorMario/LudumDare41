#pragma once
class AABB
{
public:
  AABB();
  AABB( vec3 a_p );
  AABB( vec3 a_p1, vec3 a_p2 );
  AABB( const AABB& rhs );

  AABB Union( const vec3& a_p ) const;
  AABB Union( const AABB& a_aabb2 ) const;

  bool Overlaps( const AABB& a_aabb ) const;
  bool Inside( const vec3& a_p ) const;

  void Expand( float delta );

  float SurfaceArea() const;
  float Volume() const;

  int MaxExtent() const;
  vec3 Center()const;
  //vec3 Lerp( float tx, float ty, float tz ) const;

  vec3 Offset( const vec3& v ) const;

  void BoundingSphere( float* r, vec3* p ) const;

  bool Intersect( const Ray& ray, const vec3& invDir, const int dirIsNeg[3] ) const;
  bool Intersect( const Ray& ray ) const;

  inline const bool Contains( const AABB& rhs ) const
  {
    return
      min.x < rhs.min.x && min.y < rhs.min.y && min.z < rhs.min.z &&
      max.x > rhs.max.x && max.y > rhs.max.y && max.z > rhs.max.z;
  }

  inline const bool operator==( const AABB& rhs ) const
  {
    AABB temp = *this;
    temp.Expand( EPSILON ); // apply an error margin
    bool fits_in = temp.Contains( rhs );
    temp.Expand( EPSILON * -2.0f );
    return fits_in && rhs.Contains( temp );
  }

  inline const bool operator!=( const AABB& rhs ) const
  {
    return !( *this == rhs );
  }

  inline const vec3& operator[]( const int i ) const
  {
    return ( &min )[i];
  }
  inline vec3& operator[]( const int i )
  {
    return ( &min )[i];
  }

  inline vec3 Corner( int i ) const
  {
    return vec3( ( *this )[i & 1].x,
      ( *this )[i & 2 ? 1 : 0].y,
      ( *this )[i & 4 ? 1 : 0].z );
  }

public:
  vec3 min, max;
};