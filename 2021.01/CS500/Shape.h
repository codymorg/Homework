#pragma once
#include "Material.h"
#include "Ray.h"
#include <Eigen/StdVector>
#include <Eigen_unsupported/Eigen/BVH>
#include <glm/glm/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>
const float                         INF = std::numeric_limits<float>::max();
typedef Eigen::AlignedBox<float, 3> Bbox; // The BV type provided by Eigen

static Eigen::Vector3f VecToEigen(const glm::vec3& vec)
{
  return Eigen::Vector3f(vec.x, vec.y, vec.z);
}
static glm::vec3 EigenToVec(const Eigen::Vector3f& evec)
{
  return glm::vec3(evec.x(), evec.y(), evec.z());
}

static glm::vec3 vec3Min(const glm::vec3& left, const glm::vec3& right)
{
  return glm::vec3(
    left.x < right.x ? left.x : right.x, left.y < right.y ? left.y : right.y, left.z < right.z ? left.z : right.z);
}
static glm::vec3 vec3Max(const glm::vec3& left, const glm::vec3& right)
{
  return glm::vec3(
    left.x > right.x ? left.x : right.x, left.y > right.y ? left.y : right.y, left.z > right.z ? left.z : right.z);
}

class Shape
{
public:
  static const int NO_COLLISION = -1;
  const float      EPSILON      = 0.00001f;

  // Motion blur stuff
  bool                   motionBlur = false;
  float                  random_t = 0.0f;
  std::vector<glm::vec3> cp;

  Shape(const std::string name) : name_(name){};
  Shape(const std::string& name, const glm::vec3& position) : name_(name), pos_(position){};

  virtual float     intersect(const Ray& ray, glm::vec3& norm, bool debug = false) = 0;
  virtual glm::vec3 getpos()
  {
    return pos_;
  };
  virtual void bounding_box() = 0;
  static bool  planeIntersection(const Ray&       ray,
                                 const glm::vec3& norm,
                                 const glm::vec3  planarPoint,
                                 glm::vec3&       collisionPoint);
  static bool  isInBounds(const glm::vec3& point, const glm::vec3& minPoint, const glm::vec3& maxPoint);

  static void PrintVec3(std::string name, glm::vec3 vec, bool nl = true)
  {
    printf("%s : (%.3f, %.3f, %.3f)", name.c_str(), vec.x, vec.y, vec.z);
    if (nl)
      printf("\n");
  }
  void addMaterial(const MyMaterial& material)
  {
    mat_ = material;
  };
  const std::string name()
  {
    return name_;
  };

  MyMaterial mat_;
  Bbox       bbox;

private:
  glm::vec3   pos_;
  std::string name_;
};

class Sphere : public Shape
{
public:
  Sphere(float x, float y, float z, float r, int id) : Shape("Sphere" + std::to_string(id), {x, y, z}), radius(r)
  {
    bounding_box();
  };

  glm::vec3 getpos() override;

  float intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  void  bounding_box();

  float radius = 0;

private:
};

class Box : public Shape
{
public:
  Box(float x, float y, float z, float vx, float vy, float vz, int id);
  Box(glm::vec3 minV, glm::vec3 maxV, int id);
  void initBox(float x, float y, float z, float vx, float vy, float vz);

  float intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  void  bounding_box();

  bool isInBox(const glm::vec3& point);

  glm::vec3              diagonal;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> points;

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

  float intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  void  bounding_box();

  bool cramers(const std::vector<glm::vec3>& equations, glm::vec3& solution);
  void printMat3(const glm::mat3x3& mat);

  float     radius;
  glm::vec3 axis;

private:
};

class VertexData
{
public:
  glm::vec3 pnt;
  glm::vec3 nrm;
  glm::vec3 tex;
  glm::vec3 tan;
  VertexData(const glm::vec3& p, const glm::vec3& n, const glm::vec3& t, const glm::vec3& a)
    : pnt(p), nrm(n), tex(t), tan(a)
  {
  }
};
struct MeshData
{
  std::vector<VertexData> vertices;
  std::vector<glm::vec3>  triangles;
  std::string             fileName;
};

class Model : public Shape
{
public:
  Model(std::string name, MeshData* data, const std::vector<Shape*>& shapes, int id);
  ~Model();

  float     intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  void      bounding_box();
  bool      isInTriangle(int triNumber, const Ray& ray, bool debug = false);
  void      makeBoundingHierarchy(const std::vector<Shape*>& shapes);
  glm::vec3 centroidOfTri(const glm::vec3& tri);

  std::pair<glm::vec3, glm::vec3> boundsOfTri(const glm::vec3& tri);
  std::pair<glm::vec3, glm::vec3> boundsOfTri(const glm::vec3& tri, const glm::vec3& tri2, const glm::vec3& tri3);

  MeshData*                      meshData = nullptr;
  std::vector<Box*>              hierarchy;
  std::vector<std::vector<int>>  triIndicies;
  Eigen::KdBVH<float, 3, Shape*> Tree;

  class Minimizer
  {
  public:
    typedef float Scalar; // KdBVH needs Minimizer::Scalar defined
    Ray           ray;
    // Stuff to track the minimal t and its intersection info
    // Constructor
    Minimizer(const Ray& r) : ray(r)
    {
    }
    // Called by BVMinimize to intersect the ray with a Shape. This
    // should return the intersection t, but should also track
    // the minimum t and it's corresponding intersection info.
    // Return INF to indicate no intersection.
    float minimumOnObject(Shape* obj);
    // Called by BVMinimize to intersect the ray with a Bbox and
    // returns the t value. This should be similar to the already
    // written box (3 slab) intersection. (The difference being: a
    // distance of zero should be returned if the ray starts within the Bbox.)
    // Return INF to indicate no intersection.
    float minimumOnVolume(const Bbox& box);
  };
};
