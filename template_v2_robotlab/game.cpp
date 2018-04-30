#include "precomp.h" // include (only) this in every .cpp file

Rasterizer rasterizer;
Raytracer raytracer;
Camera camera;
vec3 position;
bool raytracing = false;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	// initialize rasterizer
	rasterizer.Init( screen );
	// setup camera (note: in ogl/glm, z for 'far' is -inf)
	position = vec3( 0, 0, 8 );
	camera.SetPosition( position );
	camera.LookAt( vec3( 0, 0, 0 ) );
	// initialize scene
	rasterizer.scene->Add( "assets/unity_full/unityScene.obj" );
  //rasterizer.scene->Add( "assets/test/Snow covered CottageOBJ.obj" );
	//rasterizer.scene->Add( "assets/pacman/pacman_close.obj" );
  raytracer.Init(screen, rasterizer.scene);
}

// -----------------------------------------------------------
// Input handling
// -----------------------------------------------------------
void Game::HandleInput( float dt )
{
	if (GetAsyncKeyState( 'W' )) position += camera.GetForward() * dt * 0.4f;
	if (GetAsyncKeyState( 'S' )) position -= camera.GetForward() * dt * 0.4f;
	if (GetAsyncKeyState( 'A' )) position -= camera.GetRight() * dt * 0.4f;
	if (GetAsyncKeyState( 'D' )) position += camera.GetRight() * dt * 0.4f;
	if (GetAsyncKeyState( 'R' )) position += camera.GetUp() * dt * 0.4f;
	if (GetAsyncKeyState( 'F' )) position -= camera.GetUp() * dt * 0.4f;
	camera.SetPosition( position );
	if (GetAsyncKeyState( VK_LEFT )) camera.LookAt( camera.GetPosition() + camera.GetForward() - dt * 0.02f * camera.GetRight() );
	if (GetAsyncKeyState( VK_RIGHT )) camera.LookAt( camera.GetPosition() + camera.GetForward() + dt * 0.02f * camera.GetRight() );
	if (GetAsyncKeyState( VK_UP )) camera.LookAt( camera.GetPosition() + camera.GetForward() - dt * 0.02f * camera.GetUp() );
	if (GetAsyncKeyState( VK_DOWN )) camera.LookAt( camera.GetPosition() + camera.GetForward() + dt * 0.02f * camera.GetUp() );
  static bool pressed = false;
  if ( GetAsyncKeyState( VK_SPACE ) && pressed == false )
  {
    raytracing = !raytracing;
    pressed = true;
  }
  else
  {
    pressed = false;
  }
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	HandleInput( 1 );
	screen->Clear( 0 );
  if ( raytracing )
  {
    raytracer.Render( camera );
  }
  else
  {
    rasterizer.Render( camera );
  }
}