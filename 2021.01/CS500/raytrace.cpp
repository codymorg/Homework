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
Vec3 ScaleVec3(const Vec3& vec, const Vec3& scale)
{
  return {
    vec.x() * scale.x(),
    vec.y() * scale.y(),
    vec.z() * scale.z(),
  };
}
Scene::Scene()
{
}

void Scene::traceImage(Color* image)
{
  SetAxes();

  float maxDist = 14.257924f;

  Color color(0, 0, 0);

#ifndef _DEBUG
#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
#endif
  for (int y = 0; y < height; y++)
  {
    fprintf(stderr, "Rendering %4d / %4d\r ", y, height - 1);

    for (int x = 0; x < width; x++)
    {
      // build ray
      Ray ray = castRay(x, y);

      // intersect with every object
      float shortest = INFINITY;
      Vec3  shortColor;

      int  i       = 0;
      bool isLight = false;
      for (auto shape : shapes)
      {
        Intersection inter;
        auto         dist = shape->intersect(ray, inter);
        if (dist < shortest)
        {
          shortest = dist;
          isLight  = shape->mat.isLight;

          shortColor = calculateLighting(inter);
          /*
          shortColor = shape->mat.diffuse;
          shortColor = inter.normal;
          shortColor = {dist / maxDist, dist / maxDist, dist / maxDist};
          */
        }
      }

      // return closest
      if (shortest != INFINITY)
        color = shortColor;

      image[y * width + x] = color;
    }
  }
}

void Scene::tracePath(Color* image, const int pass)
{
  SetAxes();
  // transform coordinates

#ifndef _DEBUG
#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
#endif
  for (int y = 0; y < height; y++)
  {
    fprintf(stderr, "Rendering %4d / %4d\r ", y, height - 1);
    for (int x = 0; x < width; x++)
    {
      Vec3 c(0, 0, 0);
      Vec3 W(1, 1, 1);
      for (int p = 0; p < pass; p++)
      {

        // intersect with every object
        Ray          ray = castRay(x, y);
        Intersection P   = findIntersection(ray);

        // no intersection
        if (P.t == INFINITY)
          image[y * width + x] = Color(c);

        // found a light
        else if (P.object && P.object->mat.isLight)
          image[y * width + x] = Color(evalRadiance(P));

        // spawn new rays
        else
        {
          while (russianRoulette())
          {
            Vec3 N = P.normal;

            // cast a new ray
            Vec3 w = sampleBRDF(N);
            Ray  newRay(P.point, w);

            // get new ray's intersection
            Intersection Q = findIntersection(newRay);
            if (!Q.object)
              break;

            auto f = evalScattering(P, w);
            auto p = pdfBrdf(N, w) * conservationEnergyValue;
            if (p < EPSILON)
              break;

            W = ScaleVec3(W, f / p);

            if (Q.object->mat.isLight)
            {
              auto e      = evalRadiance(Q);
              Vec3 newVec = {W.x() * e.x(), W.y() * e.y(), W.z() * e.z()};
              c += newVec;
              break;
            }
            else
            {
              P = Q;
            }
          }
          Color color(c);
          image[y * width + x] += color;
        }
      }
    }
  }
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

Vec3 Scene::calculateLighting(const Intersection& inter)
{
  Vec3         diffuse = inter.object->mat.diffuse;
  Vec3         norm    = inter.normal;
  Vec3         objPos  = inter.point;
  const Shape& object  = *inter.object;
  Vec3         total   = {0, 0, 0};
  Vec3         E       = (camera.pos - objPos).normalized();
  for (auto hit : shapes)
  {
    if (hit->mat.isLight)
    {
      Vec3  L  = (hit->pos - objPos).normalized();
      Vec3  H  = (L + E).normalized();
      float NL = norm.dot(L) > 0.0f ? norm.dot(L) : 0.0f;
      float HN = H.dot(norm) > 0.0 ? H.dot(norm) : 0.0f;

      float a = std::pow(HN, object.mat.alpha);
      Vec3  b = diffuse * NL / PI;
      Vec3  c = object.mat.alpha / (2 * PI) * object.mat.specular;
      total += ScaleVec3(hit->mat.diffuse, b + c * a);
    }
  }

  return total;
}

void Scene::SetAxes()
{
  float       ry = camera.frustumRatio;
  float       rx = ry * width / height;
  Quaternionf Q(camera.rot.w(), camera.rot.x(), camera.rot.y(), camera.rot.z());
  X = rx * Q._transformVector(Vector3f::UnitX());
  Y = ry * Q._transformVector(Vector3f::UnitY());
  Z = -1 * Q._transformVector(Vector3f::UnitZ());
}

Ray Scene::castRay(int x, int y)
{
  float    r0   = myrandom(RNGen);
  float    r1   = myrandom(RNGen);
  float    dx   = 2.0 * (x + r0) / width - 1.0;
  float    dy   = 2.0 * (y + r1) / height - 1.0;
  Vector3f dirE = dx * X + dy * Y + Z;
  Vec3     dir(dirE.x(), dirE.y(), dirE.z());

  return Ray(camera.pos, dir);
}

Intersection Scene::findIntersection(const Ray& ray)
{
  Intersection Q;

  for (auto shape : shapes)
  {
    Intersection inter;
    auto         dist = shape->intersect(ray, inter);
    if (dist < Q.t)
    {
      Q = inter;
    }
  }

  return Q;
}

bool Scene::russianRoulette(float leq)
{
  float value = myrandom(RNGen);
  return value <= leq;
}

Vec3 Scene::sampleBRDF(const Vec3& N)
{
  float p = myrandom(RNGen);
  float q = myrandom(RNGen);

  return sampleLobe(N, sqrt(p), 2 * PI * q);
}

Vec3 Scene::sampleLobe(const Vec3 N, float c, float phi)
{
  float s = sqrt(1 - c * c);
  Vec3  K(s * cos(phi), s * sin(phi), c);
  auto  q = Quaternionf::FromTwoVectors(Vector3f::UnitZ(), N);

  return q._transformVector(K);
}

Vec3 Scene::evalScattering(const Intersection& ir, const Vec3& w)
{
  return std::abs(ir.normal.dot(w)) * ir.object->mat.diffuse / PI;
}

float Scene::pdfBrdf(const Vec3& N, const Vec3& w)
{
  return std::abs(N.dot(w)) / PI;
}

Vec3 Scene::evalRadiance(const Intersection& ir)
{
  return ir.object->mat.diffuse;
}

Intersection Scene::sampleLight()
{
  std::uniform_real_distribution<> r0(0, lights.size());

  int          i         = r0(RNGen);
  Shape*       light     = lights[i];
  Intersection ir        = sampleSphere(light->pos, dynamic_cast<Sphere*>(light)->radius);
  ir.object              = light;
  ir.object->mat.isLight = true;

  return ir;
}

Intersection Scene::sampleSphere(const Vec3& C, float R)
{
  float        r1 = myrandom(RNGen);
  float        r2 = myrandom(RNGen);
  float        z  = 2 * r1 - 1;
  float        r  = std::sqrt(1 - z * z);
  float        a  = 2 * PI * r2;
  Intersection ir;
  ir.normal = Vec3(r * cos(a), r * sin(a), z);
  ir.point  = C + R * ir.normal;

  return ir;
}

float Scene::pdfLight(const Intersection& ir)
{
  auto  light = dynamic_cast<Sphere*>(ir.object);
  float sa    = 4 * PI * light->radius * light->radius;

  return 1 / (sa * lights.size());
}

float Scene::geometryFactor(Intersection A, Intersection B)
{
  auto D = A.point - B.point;
  return std::abs(A.normal.dot(D) * B.normal.dot(D) / std::pow(D.dot(D), 2));
}
