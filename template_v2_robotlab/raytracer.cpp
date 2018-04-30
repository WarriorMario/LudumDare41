#include "precomp.h"


Tmpl8::Raytracer::~Raytracer()
{
}

void Tmpl8::Raytracer::Init( Surface * screen, Scene* scene)
{
  m_screen = screen;
  m_scene = scene;

  printf("Started top level bvh construction /n");
  bvh = new TopLevelBVH(m_scene->bvhList, m_scene->root);

  m_test.pos[0] = vec3(0.0f, 4.0f, -5.0f);
  m_test.pos[1] = vec3(4.0f, 0.0f, -5.0f);
  m_test.pos[2] = vec3(0.0f, 0.0f, -5.0f);
  
  m_test.norm[0] = vec3(0.0f, 0.0f, 1.0f);
  m_test.norm[1] = vec3(0.0f, 0.0f, 1.0f);
  m_test.norm[2] = vec3(0.0f, 0.0f, 1.0f);
  
  m_test.uv[0] = vec2(0.0f, 0.0f);
  m_test.uv[1] = vec2(0.0f, 0.0f);
  m_test.uv[2] = vec2(0.0f, 0.0f);

  m_light.pos = vec3(0.f, 5.3f, 0.f);
  m_light.col = (FColors::Magenta + FColors::Cyan) * 0.5f;
}

bool Tmpl8::Raytracer::Trace(Ray & ray, Result& res)
{
  return bvh->Traverse(ray, res);
}

bool Tmpl8::Raytracer::IsOccluded(Ray & ray)
{
  return bvh->TraverseShadow(ray);
}

  std::function<void( int )> func = RenderJob::RenderTile;
void Tmpl8::Raytracer::Render( Camera & camera )
{
  RenderJob::InitData( this, camera );
  ParallelFor( func, SCRWIDTH*SCRHEIGHT / packetSize, 20000000 );
}

RenderJob::RenderJobData RenderJob::data;


void Tmpl8::RenderJob::InitData( Raytracer* tracer, Camera& camera )
{
  // Camera and screen
  data.camPos = camera.GetPosition();
  float screenDistance = 2.f;

  data.p0 = data.camPos + ( camera.transform*vec4( -1, 1, -screenDistance, 0 ) ).xyz;
  data.p1 = data.camPos + ( camera.transform*vec4( 1, 1, -screenDistance, 0 ) ).xyz;
  data.p2 = data.camPos + ( camera.transform*vec4( -1, -1, -screenDistance, 0 ) ).xyz;
  data.raytracer = tracer;
}
thread_local static int randints;

vec3 DiffuseReflection(vec3 norm)
{
  vec3 point = vec3( ( ( rand() ) - RAND_MAX / 2 ) / (RAND_MAX / 2.f), ( ( rand() ) - RAND_MAX / 2 ) / (RAND_MAX / 2.f), ( ( rand() ) - RAND_MAX / 2 ) / (RAND_MAX / 2.f ));
  while ( point.sqrLentgh() > 1.0f )
  {
    point = vec3( ( ( rand() ) - RAND_MAX / 2 ) / ( RAND_MAX / 2.f ), ( ( rand() ) - RAND_MAX / 2 ) / ( RAND_MAX / 2.f ), ( ( rand() ) - RAND_MAX / 2 ) / ( RAND_MAX / 2.f ) );
  }
  point.normalize();
  if ( dot( norm, point ) < 0.f )
  {
    return -point;
  }
  return point;
}
FColor Sample( RenderJob::RenderJobData& data, Ray& ray, Result& res, int maxDepth)
{
  if ( maxDepth == 4 )
  {
    return FColor( 0XAAAAAAAu );
  }
  if ( data.raytracer->Trace( ray, res ) == true )
  {
    if ( res.mesh->material->diffuse == 0 )
    {
      return FColor( 0xffffffff );
    }
    vec3 reflected = DiffuseReflection( res.norm );
    FColor BRDF( FColor(0x000000ffu ));// Get colour from texture
    BRDF /= PI;
    vec3 hitpoint = ray.pos + ray.dir * ray.len;
    vec3 norm = res.norm;
    hitpoint += reflected * EPSILON;
    Ray newRay = Ray( hitpoint, reflected, 100000.f );
    FColor color = Sample( data, newRay, res , maxDepth +1) * dot( norm, reflected );
    return BRDF * PI * 2.f*color;
  
  }
  return FColor(0X00000000u);
}

void Tmpl8::RenderJob::RenderTile( int index )
{
  const int startY = (index / (SCRWIDTH / packetWidth ))*packetHeight;
  const int startX = (index % (SCRWIDTH /packetWidth))*packetWidth;
  for ( int y = startY; y < startY+packetHeight; ++y )
  {
    for ( int x = startX; x < startX + packetWidth; ++x )
    {
      float u = ( float )x / data.raytracer->m_screen->GetWidth();
      float v = ( float )y / data.raytracer->m_screen->GetHeight();
      vec3 pointOnScreen = data.p0 + ( data.p1 - data.p0 )*u + ( data.p2 - data.p0 )*v;
      vec3 rayDirection = pointOnScreen - data.camPos;
      Ray ray = Ray( data.camPos, rayDirection.normalized(), 10000000.0f );

      Result res;
      data.raytracer->m_screen->GetBuffer()[y*SCRWIDTH + x] = Sample( data, ray, res ,0).To32Bit();

    }
  }
}
