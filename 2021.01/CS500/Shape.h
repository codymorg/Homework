#pragma once
#include "Material.h"
#include "Ray.h"
#include <glm/glm/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>

class Shape
{
public:
  static const int NO_COLLISION = -1;
  const float      EPSILON      = 0.00001f;

  Shape(const std::string name) : name_(name){};
  Shape(const std::string& name, const glm::vec3& position) : name_(name), pos_(position){};

  virtual float intersect(const Ray& ray, glm::vec3& norm, bool debug = false) = 0;
  static bool   planeIntersection(const Ray&       ray,
                                  const glm::vec3& norm,
                                  const glm::vec3  planarPoint,
                                  glm::vec3&       collisionPoint);

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

  glm::vec3  pos_;
  MyMaterial mat_;


private:
  std::string name_;
};

class Sphere : public Shape
{
public:
  Sphere(float x, float y, float z, float r, int id) : Shape("Sphere" + std::to_string(id), {x, y, z}), radius(r){};

  float intersect(const Ray& ray, glm::vec3& norm, bool debug = false);

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
  bool  isInBox(const glm::vec3& point);

  glm::vec3              diagonal;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec3> points;

private:
};

class Cylinder : public Shape
{
public:
  Cylinder(float x, float y, float z, float ax, float ay, float az, float r, int id)
    : Shape("Cylinder" + std::to_string(id), {x, y, z}), axis(ax, ay, az), radius(r){};

  float intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  bool  cramers(const std::vector<glm::vec3>& equations, glm::vec3& solution);
  void  printMat3(const glm::mat3x3& mat);

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
  Model(std::string name, MeshData* data, int id);
  ~Model();

  float     intersect(const Ray& ray, glm::vec3& norm, bool debug = false);
  bool      isInTriangle(int triNumber, const Ray& ray, bool debug = false);
  void      makeBoundingHierarchy();
  glm::vec3 centroidOfTri(const glm::vec3& tri);
  std::pair<glm::vec3, glm::vec3> boundsOfTri(const glm::vec3& tri);
  std::pair<glm::vec3, glm::vec3> boundsOfTri(const glm::vec3& tri, const glm::vec3& tri2, const glm::vec3& tri3);

  MeshData*         meshData = nullptr;
  std::vector<Box*> hierarchy;
  std::vector<std::vector<int>> triIndicies;
};
