#pragma once

class TopLevelBVH;
namespace Tmpl8 
{

// -----------------------------------------------------------
// Raytracer class
// to be build
// -----------------------------------------------------------
class Raytracer
{
public:
	// constructor / destructor
	Raytracer() : m_scene( 0 ) {}
	~Raytracer();
	// methods
	void Init( Surface* screen, Scene* scene );
	bool Trace( Ray& ray, Result& res );
	bool IsOccluded( Ray& ray );
  Pixel Shade(Result& res, Ray& ray);
	void Render( Camera& camera );
	// data members
	Scene* m_scene;	
  Surface* m_screen;
  TopLevelBVH* bvh;

  Triangle m_test;
  Light m_light;
};

class RenderJob
{
public:
  static void InitData(Raytracer* raytracer, Camera & camera );
  static void RenderTile( int index );

  // Shared job data
  static struct RenderJobData
  {
    Raytracer* raytracer;
    vec3 camPos;
    vec3 p0;
    vec3 p1;
    vec3 p2;
  } data;
};

}; // namespace Tmpl8