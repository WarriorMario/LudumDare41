// add your includes to this file instead of to individual .cpp files
// to enjoy the benefits of precompiled headers:
// - fast compilation
// - solve issues with the order of header files once (here)
// do not include headers in header files (ever).

#define SCRWIDTH		512
#define SCRHEIGHT		512
// #define FULLSCREEN
// #define ADVANCEDGL	// faster if your system supports it

#include <inttypes.h>
extern "C" 
{ 
#include "glew.h" 
}
#include "gl.h"
#include "io.h"
#include <fstream>
#include <stdio.h>
#include "fcntl.h"
#include "SDL.h"
#include "wglext.h"
#include "freeimage.h"
#include "math.h"
#include "stdlib.h"
#include "emmintrin.h"
#include "immintrin.h"
#include "windows.h"
#include "template.h"
#include "surface.h"
#include "threads.h"
#include <assert.h>
#include <vector>
#include <map>
#include <memory>
#include "ParallelForLoop.h"

using namespace std;
using namespace Tmpl8;
#include "Source/FColor.h"
#include "Source/Light.h"
#include "Source/Geometry/triangle.h"
#include "Source/result.h"
#include "Source/AABB.h"
#include "Source/BVH/DynamicBVH.h"
#include "Source/BVH/TopLevelBVH.h"
#include "Source/Ray.h"
#include "rasterizer.h"
#include "raytracer.h"
#include "game.h"