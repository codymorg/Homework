#pragma once
#include "common.h"
#include "Ray.h"
#include "MyMaterial.h"
class Shape
{
public:
  Shape(const string& ShapeName, Vec3 position = Vec3::Zero());

  virtual float interset(const Ray& ray, Intersection& inter) = 0;
  void addMaterial(const MyMaterial& material);

  static bool   planeIntersection(const Ray& ray,
    const Vec3& norm,
    const Vec3  planarPoint,
    Vec3& collisionPoint);

  Vec3 pos;
  MyMaterial mat;
  string name;
  Bbox bbox;
  const float EPSILON = 0.00001f;
  const float NO_COLLISION = INFINITY;
};

class Sphere : public Shape
{
public:
  Sphere(float x, float y, float z, float r, int id) : Shape("Sphere" + std::to_string(id), { x, y, z }), radius(r)
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
    : Shape("Cylinder" + std::to_string(id), { x, y, z }), axis(ax, ay, az), radius(r)
  {
    bounding_box();
  };

  float intersect(const Ray& ray, Intersection& inter);
  void  bounding_box();

  bool cramers(const std::vector<Vec3>& equations, Vec3& solution);
  void printMat3(const Mat3& mat);

  float     radius;
  Vec3 axis;

private:
};