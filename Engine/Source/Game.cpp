/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

#include "SceneTraversal/Ray.h"
#include "SceneTraversal/Intersection.h"
#include "Utilities/ParallelForLoop.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
  Load();
  Optimize();
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::Load()
{
  // utah teapot
#define _WIN32_WCE
#include "UtahTeapot.h"
#undef _WIN32_WCE

  Mesh teapot;
  teapot.SetTransform(Transform::CreateTranslate(Vec3(kZeroF)));
  for (int32 i = 0; i < numFaces; ++i)
  {
    int32 vIndex = 0;

    Vec3 p[3];
    Vec3 n[3];
    Vec2 uv[3];

    for (int32 j = 1; j < 4; ++j, ++vIndex)
    {
      p[vIndex] = Vec3(
        vertices[faces[i][j]][0],
        vertices[faces[i][j]][1],
        vertices[faces[i][j]][2]);

      n[vIndex] = Vec3(
        normals[faces[i][j + 3]][0],
        normals[faces[i][j + 3]][1],
        normals[faces[i][j + 3]][2]);

      uv[vIndex] = Vec2((
        vertices[faces[i][j]][0] + Float(4.5)) / Float(9.643),
        vertices[faces[i][j]][1] / Float(4.725));
    }
    teapot.AddTriangle(Triangle(p, n, uv, FColors::White));
  }
  scene_.AddMesh(teapot);
}

void Game::Optimize()
{
  scene_.Optimize();
}

void Game::UpdateModel()
{
}

void Game::ComposeFrame()
{
  // precalculate
  int32 sh = gfx.ScreenHeight;
  int32 sw = gfx.ScreenWidth;
  int32 tot_pixels = sh * sw;
  Float shf = Float(sh);
  Float swf = Float(sw);
  Float distance = Float(1.8);
  Float aspect = Float(sw) / Float(sh);
  Float inv_aspect = 1.0f / aspect;
  Float neg_distance = -distance;

  // left top
  Vec3 p0 = Vec3(-1, inv_aspect, neg_distance);
  // right top
  Vec3 p1 = Vec3(1, inv_aspect, neg_distance);
  // left bottom
  Vec3 p2 = Vec3(-1, -inv_aspect, neg_distance);

  std::function<void(int32)> func = [&](int32 i)
  {
    int32 x = i % sw;
    Float ndc_x = Float(x) / swf;
    int32 y = i / sw;
    Float ndc_y = Float(y) / shf;

    // calculate the ray
    Vec3 planepos = p0 + (p1 - p0) * ndc_x + (p2 - p0) * ndc_y;
    Ray ray = Ray(Vec3(kZeroF, kZeroF, Float(15.0)), planepos.Normalized());

    Intersection hit;
    if (scene_.Intersect(ray, hit))
    {
      gfx.PutPixel(x, y, Colors::White);
    }
    else
    {
      gfx.PutPixel(x, y, Colors::Black);
    }
  };
  ParallelFor(func, tot_pixels, 64);

  //for (int32 y = 0; y < sh; ++y)
  //{
  //  Float ndc_y = Float(y) / shf;
  //
  //  for (int32 x = 0; x < sw; ++x)
  //  {
  //    Float ndc_x = Float(x) / swf;
  //
  //    // calculate the ray
  //    Vec3 planepos = p0 + (p1 - p0) * ndc_x + (p2 - p0) * ndc_y;
  //    Ray ray = Ray(Vec3(kZeroF, kZeroF, Float(10.0)), planepos.Normalized());
  //
  //    Intersection hit;
  //    if (scene_.Intersect(ray, hit))
  //    {
  //      gfx.PutPixel(x, y, Colors::White);
  //    }
  //    else
  //    {
  //      gfx.PutPixel(x, y, Colors::Black);
  //    }
  //  }
  //}
}
