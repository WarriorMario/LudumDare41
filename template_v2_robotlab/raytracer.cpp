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
  ParallelFor( func, SCRWIDTH*SCRHEIGHT / packetSize, 2 );
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
      if ( data.raytracer->Trace( ray, res ) )
      {
        vec3 pos = res.tri->GetPosAtB( res.bary_coords );
        vec3 norm = res.tri->GetNormalAtB( res.bary_coords );

        vec3 hitpoint = ray.pos + ray.dir * ray.len;
        vec3 lightDir = data.raytracer->m_light.pos - hitpoint;
        hitpoint += norm * EPSILON;
        float length = lightDir.length();
        lightDir *= 1.0f / length;
        Ray shadow = Ray( hitpoint, lightDir, length );
        if ( data.raytracer->IsOccluded( shadow ) == false )
        {
          FColor li = data.raytracer->m_light.GetLightAtPoint( pos );
          float d = dot( norm, shadow.dir );
          if ( d < 0.f )
          {
            data.raytracer->m_screen->GetBuffer()[y*SCRWIDTH + x] = ( li * 0.0f ).To32Bit();
          }
          else
          {
            data.raytracer->m_screen->GetBuffer()[y*SCRWIDTH + x] = ( li * d ).To32Bit();

          }
        }
        else
        {
          data.raytracer->m_screen->GetBuffer()[y*SCRWIDTH + x] = 0xaaaaaaaa;
        }
      }
      else
      {
        data.raytracer->m_screen->GetBuffer()[y*SCRWIDTH + x] = 0;
      }
    }
  }
}
