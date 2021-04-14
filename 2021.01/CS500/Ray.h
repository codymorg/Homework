#pragma once
#include "common.h"
#include "Shape.h"
struct Ray
{
  Ray(const Vec3& origin, const Vec3& direction) :origin(origin) { dir = direction.normalized(); }
  Vec3 origin;
  Vec3 dir;
};

struct Intersection
{
  Intersection(float tt, Shape* obj, Vec3 norm, Vec3 point) : t(tt), object(obj), normal(norm), point(point) {}
  void set(float tt, Shape* obj, Vec3 norm, Vec3 intersectionPoint)
  {
    t = tt;
    object = obj;
    normal = norm;
    point = intersectionPoint;
  }
  void invalid()
  {
    t = INFINITY;
    object = nullptr;
    normal = Vec3::Zero();
    point = Vec3::Zero();
  }

  float t; // on ray
  Shape* object;
  Vec3 normal; //N
  Vec3 point; //P
};

class Interval
{
public:
  Interval()
  {
    t0 = 0;
    t1 = INFINITY;
  }

  Interval(float t, float tt, Vec3 n, Vec3 nn)
  {
    if (t < tt)
    {
      t0 = t;
      t1 = tt;
      normal0 = n;
      normal1 = nn;
    }
    else
    {
      t0 = tt;
      t1 = t;
      normal0 = nn;
      normal1 = n;
    }

  }

  void empty()
  {
    t0 = 0;
    t1 = -1;
    normal0 = Vec3::Zero();
    normal1 = Vec3::Zero();
  }

  void intersect(const Interval& other)
  {
    throw "todo";
  }
private:
  float t0,t1;
  Vec3 normal0, normal1;
};
