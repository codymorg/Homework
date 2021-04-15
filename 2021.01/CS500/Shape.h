#pragma once
#include "common.h"
#include "MyMaterial.h"
#include "Ray.h"

typedef class Shape Shape;
struct Intersection
{
  Intersection(float tt, Shape* obj, Vec3 norm, Vec3 point) : t(tt), object(obj), normal(norm), point(point)
  {
  }
  void set(float tt, Shape* obj, Vec3 norm, Vec3 intersectionPoint)
  {
    t      = tt;
    object = obj;
    normal = norm;
    point  = intersectionPoint;
  }
  void invalid()
  {
    t      = INFINITY;
    object = nullptr;
    normal = Vec3::Zero();
    point  = Vec3::Zero();
  }

  float  t; // on ray
  Shape* object;
  Vec3   normal; // N
  Vec3   point;  // P
};

class Shape
{
public:
  Shape(const string& ShapeName, Vec3 position = Vec3(0,0,0));

  virtual float intersect(const Ray& ray, Intersection& inter) = 0;

  static bool planeIntersection(const Ray& ray, const Vec3& norm, const Vec3 planarPoint, Vec3& collisionPoint);

  Vec3        pos;
  MyMaterial  mat;
  string      name;
  Bbox        bbox;
  const float EPSILON      = 0.00001f;
  const float NO_COLLISION = INFINITY;
};

class Sphere : public Shape
{
public:
  Sphere(float x, float y, float z, float r, int id) : Shape("Sphere" + std::to_string(id), {x, y, z}), radius(r)
  {
    bounding_box();
  };

  float intersect(const Ray& ray, Intersection& inter);
  void  bounding_box();

  float radius = 0;

private:
};

class Box : public Shape
{
public:
  Box(float x, float y, float z, float vx, float vy, float vz, int id);
  Box(Vec3 minV, Vec3 maxV, int id);
  void initBox(float x, float y, float z, float vx, float vy, float vz);

  float intersect(const Ray& ray, Intersection& inter);
  void  bounding_box();

  bool isInBox(const Vec3& point);

  Vec3              diagonal;
  std::vector<Vec3> normals;
  std::vector<Vec3> points;

private:
};

class Cylinder : public Shape
{
public:
  Cylinder(float x, float y, float z, float ax, float ay, float az, float r, int id)
    : Shape("Cylinder" + std::to_string(id), {x, y, z}), axis(ax, ay, az), radius(r)
  {
    bounding_box();
  };

  float intersect(const Ray& ray, Intersection& inter);
  void  bounding_box();

  bool cramers(const std::vector<Vec3>& equations, Vec3& solution);

  float radius;
  Vec3  axis;

private:
};
