//////////////////////////////////////////////////////////////////////
// Provides the framework for a raytracer.
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
using std::cout;

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
  Scene::imageTypeNames = {"TestOnly", "DepthOnly", "DifuseOnly", "NormalOnly", "LitOnly", "All"};
}

Scene::~Scene()
{
  for (auto& obj : shapes)
  {
    delete (obj);
    obj = nullptr;
  }
}

void Scene::Finit()
{
}

void Scene::triangleMesh(MeshData* mesh)
{
  Model* obj = new Model(mesh->fileName, mesh, shapes.size());
  obj->addMaterial(myMaterial);
  shapes.push_back(obj);
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

vec3 Scene::calculateLighting(const vec3& diffuse, const vec3& norm, const vec3& objPos, const Shape& object)
{
  vec3 total;
  vec3 E = glm::normalize(camera.pos_ - objPos);
  for (auto light : shapes)
  {
    if (light->mat_.isLight)
    {
      vec3  L  = glm::normalize(light->pos_ - objPos);
      vec3  H  = glm::normalize(L + E);
      float NL = std::max(glm::dot(norm, L), 0.0f);
      float HN = std::max(glm::dot(H, norm), 0.0f);
      total += light->mat_.rgb() * (diffuse * NL / PI + object.mat_.rgba.a / (2*PI) * object.mat_.specular * glm::pow(HN, object.mat_.rgba.a));
    }
  }

  return total;
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
    currentMat = new Material(Vector3f(f[1], f[2], f[3]), Vector3f(f[4], f[5], f[6]), f[7]);
    myMaterial.setMaterial(f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
    myMaterial.isLight = false;
  }

  else if (c == "light")
  {
    // syntax: light  r g b
    // The rgb is the emission of the light
    // Creates a Material instance to be picked up by successive shapes
    currentMat         = new Light(Vector3f(f[1], f[2], f[3]));
    myMaterial.isLight = true;
    myMaterial.rgba    = {f[1], f[2], f[3], 1};
  }

  else if (c == "sphere")
  {
    // syntax: sphere x y z   r
    // Creates a Shape instance for a sphere defined by a center and radius
    Sphere* obj = new Sphere(f[1], f[2], f[3], f[4], shapes.size());
    obj->addMaterial(myMaterial);
    shapes.push_back(obj);
  }

  else if (c == "box")
  {
    // syntax: box bx by bz   dx dy dz
    // Creates a Shape instance for a box defined by a corner point and diagonal vector
    Box* obj = new Box(f[1], f[2], f[3], f[4], f[5], f[6], shapes.size());
    obj->addMaterial(myMaterial);
    shapes.push_back(obj);
  }

  else if (c == "cylinder")
  {
    // syntax: cylinder bx by bz   ax ay az  r
    // Creates a Shape instance for a cylinder defined by a base point, axis vector, and radius
    Cylinder* obj = new Cylinder(f[1], f[2], f[3], f[4], f[5], f[6], f[7], shapes.size());
    obj->addMaterial(myMaterial);
    shapes.push_back(obj);
  }

  else if (c == "mesh")
  {
    // syntax: mesh   filename   tx ty tz   s   <orientation>
    // Creates many Shape instances (one per triangle) by reading
    // model(s) from filename. All triangles are rotated by a
    // quaternion (qw qx qy qz), uniformly scaled by s, and
    // translated by (tx ty tz) .
    Matrix4f modelTr =
      translate(Vector3f(f[2], f[3], f[4])) * scale(Vector3f(f[5], f[5], f[5])) * toMat4(Orientation(6, strings, f));
    ReadAssimpFile(strings[1], modelTr);
  }

  else
  {
    fprintf(stderr, "\n*********************************************\n");
    fprintf(stderr, "* Unknown command: %s\n", c.c_str());
    fprintf(stderr, "*********************************************\n\n");
  }
}

void Scene::TraceImage(Color* image, Scene::ImageType imageType, const int pass)
{
  // transform coordinates
  float       ry = camera.frustumRatio_;
  float       rx = ry * width / height;
  Quaternionf Q(camera.rot_.w, camera.rot_.x, camera.rot_.y, camera.rot_.z);
  Vector3f    X = rx * Q._transformVector(Vector3f::UnitX());
  Vector3f    Y = ry * Q._transformVector(Vector3f::UnitY());
  Vector3f    Z = -1 * Q._transformVector(Vector3f::UnitZ());

  float minV    = -1;
  float maxV    = 1;
  float maxDist = 14.257924f;

#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
  for (int y = 0; y < height; y++)
  {
    fprintf(stderr, "Rendering %4d / %4d\r ", y, height - 1);

    for (int x = 0; x < width; x++)
    {
      // build ray
      float    dx   = 2.0 * (x + 0.5) / width - 1.0;
      float    dy   = 2.0 * (y + 0.5) / height - 1.0;
      Vector3f dirE = dx * X + dy * Y + Z;
      vec3     dir(dirE.x(), dirE.y(), dirE.z());
      Ray      ray(camera.pos_, dir);

      // intersect with every object
      float shortest = INFINITY;
      vec3  shortColor;

      for (auto shape : shapes)
      {
        if (x == 389 && y == 193)
          printf("%f %f\n", x, y);

        vec3 norm;
        auto dist = shape->intersect(ray, norm);
        if (dist != Shape::NO_COLLISION && dist < shortest)
        {
          shortest = dist;
          switch (imageType)
          {
          case Scene::DepthOnly:
            shortColor = {dist / maxDist, dist / maxDist, dist / maxDist};
            break;

          case Scene::DifuseOnly:
            shortColor = shape->mat_.rgb();
            break;

          case Scene::NormalOnly:
            shortColor = glm::abs(norm);
            break;

          case Scene::LitOnly:
            shortColor = calculateLighting(shape->mat_.rgb(), norm, ray.origin_ + dist * ray.dir_, *shape);
            break;

          default:
            break;
          }
        }
      }

      // return closest
      Color color(0, 0, 0);
      if (shortest != INFINITY)
        color = {shortColor.r, shortColor.g, shortColor.b};

      image[y * width + x] = color;
    }
  }
}

void Scene::TraceTestImage(Color* image, const int pass)
{
#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
  for (int y = 0; y < height; y++)
  {
    fprintf(stderr, "Rendering %4d / %4d\r ", y, height - 1);
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
