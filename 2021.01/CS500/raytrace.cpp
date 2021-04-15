//////////////////////////////////////////////////////////////////////
// Provides the framework for a raytracer.
////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
// Includes for Windows
#include <crtdbg.h>
#include <cstdlib>
#include <limits>
#include <windows.h>
#else
// Includes for Linux
#endif

#include "geom.h"
#include "raytrace.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// A good quality *thread-safe* Mersenne Twister random number generator.
#include <random>
std::random_device               device;
std::mt19937_64                  RNGen(device());
std::uniform_real_distribution<> myrandom(0.0, 1.0);
// Call myrandom(RNGen) to get a uniformly distributed random number in [0,1].

Scene::Scene()
{
}

Quaternionf Orientation(int i, const std::vector<std::string>& strings, const std::vector<float>& f)
{
  Quaternionf q(1, 0, 0, 0); // Unit quaternion
  while (i < strings.size())
  {
    std::string c = strings[i++];
    if (c == "x")
      q *= angleAxis(f[i++] * Radians, Vector3f::UnitX());
    else if (c == "y")
      q *= angleAxis(f[i++] * Radians, Vector3f::UnitY());
    else if (c == "z")
      q *= angleAxis(f[i++] * Radians, Vector3f::UnitZ());
    else if (c == "q")
    {
      q *= Quaternionf(f[i + 0], f[i + 1], f[i + 2], f[i + 3]);
      i += 4;
    }
    else if (c == "a")
    {
      q *= angleAxis(f[i + 0] * Radians, Vector3f(f[i + 1], f[i + 2], f[i + 3]).normalized());
      i += 4;
    }
  }
  return q;
}

void Scene::Command(const std::vector<std::string>& strings, const std::vector<float>& f)
{
  if (strings.size() == 0)
    return;

  std::string c = strings[0];

  if (c == "screen")
  {
    // syntax: screen width height
    width  = int(f[1]);
    height = int(f[2]);
  }

  else if (c == "camera")
  {
    // syntax: camera x y z   ry   <orientation spec>
    // Eye position (x,y,z),  view orientation (qw qx qy qz),  frustum height ratio ry
    Quaternionf q = Orientation(5, strings, f);
    camera.setCamera(f[1], f[2], f[3], f[4], f[6], f[7], f[8], f[9]);
  }
  // camera.rot_ = {}

  else if (c == "ambient")
  {
    // syntax: ambient r g b
    // Sets the ambient color.  Note: This parameter is temporary.
    // It will be ignored once your raytracer becomes capable of
    // accurately *calculating* the true ambient light.
    ambientLight = {f[1], f[2], f[3]};
  }

  else if (c == "brdf")
  {
    // syntax: brdf  r g b   r g b  alpha
    // later:  brdf  r g b   r g b  alpha  r g b ior
    // First rgb is Diffuse reflection, second is specular reflection.
    // third is beer's law transmission followed by index of refraction.
    // Creates a Material instance to be picked up by successive shapes
    currentMat         = MyMaterial(Vector3f(f[1], f[2], f[3]), Vector3f(f[4], f[5], f[6]), f[7]);
    currentMat.isLight = false;
  }

  else if (c == "light")
  {
    // syntax: light  r g b
    // The rgb is the emission of the light
    // Creates a Material instance to be picked up by successive shapes
    currentMat.isLight = true;
    currentMat.diffuse = {f[1], f[2], f[3]};
    currentMat.alpha   = 1.0f;
  }

  else if (c == "sphere")
  {
    // syntax: sphere x y z   r
    // Creates a Shape instance for a sphere defined by a center and radius
    Sphere* obj = new Sphere(f[1], f[2], f[3], f[4], shapes.size());
    obj->mat    = currentMat;
    shapes.push_back(obj);
    if (currentMat.isLight)
      lights.push_back(obj);
  }

  else if (c == "box")
  {
    // syntax: box bx by bz   dx dy dz
    // Creates a Shape instance for a box defined by a corner point and diagonal vector
    Box* obj = new Box(f[1], f[2], f[3], f[4], f[5], f[6], shapes.size());
    obj->mat = currentMat;
    shapes.push_back(obj);
    if (currentMat.isLight)
      lights.push_back(obj);
  }

  else if (c == "cylinder")
  {
    // syntax: cylinder bx by bz   ax ay az  r
    // Creates a Shape instance for a cylinder defined by a base point, axis vector, and radius
    Cylinder* obj = new Cylinder(f[1], f[2], f[3], f[4], f[5], f[6], f[7], shapes.size());
    obj->mat      = currentMat;
    shapes.push_back(obj);
    if (currentMat.isLight)
      lights.push_back(obj);
  }

  else
  {
    fprintf(stderr, "\n*********************************************\n");
    fprintf(stderr, "* Unknown command: %s\n", c.c_str());
    fprintf(stderr, "*********************************************\n\n");
  }
}

void Scene::TraceImage(Color* image, const int pass)
{
#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
  for (int y = 0; y < height; y++)
  {

    fprintf(stderr, "Rendering %4d\r", y);
    for (int x = 0; x < width; x++)
    {
      Color color;
      float testwidth = 100;
      if ((x - width / 2) * (x - width / 2) + (y - height / 2) * (y - height / 2) < testwidth * testwidth)
        color = Color(myrandom(RNGen), myrandom(RNGen), myrandom(RNGen));
      else if (abs(x - width / 2) < 4 || abs(y - height / 2) < 4)
        color = Color(0.0, 0.0, 0.0);
      else
        color = Color(1.0, 1.0, 1.0);
      image[y * width + x] = color;
    }
  }
  fprintf(stderr, "\n");
}
