#include "Shape.h"
#include "glm/glm/geometric.hpp"
#include "glm/glm/mat3x3.hpp"
#include "glm/glm/matrix.hpp"
#include <Eigen/src/Geometry/Quaternion.h>
#include <algorithm>
using Eigen::Quaternionf;
using glm::vec3;
using std::max;
using std::min;
using std::vector;
#include <map>
using std::map;

#define DEBUG _DEBUG & 0

Bbox bounding_box(const Shape* obj)
{
  return obj->bbox; // Assuming each Shape object has its own bbox method.
}

bool Shape::planeIntersection(const Ray&       ray,
                              const glm::vec3& norm,
                              const glm::vec3  planarPoint,
                              glm::vec3&       collisionPoint)
{
  vec3  normal   = glm::normalize(norm);
  float t        = glm::dot((planarPoint - ray.origin_), normal) / glm::dot(ray.dir_, normal);
  collisionPoint = ray.origin_ + t * ray.dir_;

  return t > 0;
}

bool Shape::isInBounds(const glm::vec3& point, const glm::vec3& minPoint, const glm::vec3& maxPoint)
{
  bool xmin = point.x <= maxPoint.x;
  bool ymin = point.y <= maxPoint.y;
  bool zmin = point.z <= maxPoint.z;
  bool xmax = point.x >= minPoint.x;
  bool ymax = point.y >= minPoint.y;
  bool zmax = point.z >= minPoint.z;

  return xmin && ymin && zmin && xmax && ymax && zmax;
}

float Sphere::intersect(const Ray& ray, glm::vec3& norm, bool debug)
{
  glm::vec3 m = ray.origin_ - pos_;
  float     b = glm::dot(m, ray.dir_);
  float     c = glm::dot(m, m) - radius * radius;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
  if (c > 0.0f && b > 0.0f)
    return NO_COLLISION;

  float discr = b * b - c;

  // A negative discriminant corresponds to ray missing sphere
  if (discr < 0.0f)
    return NO_COLLISION;

  // Ray now found to intersect sphere, compute smallest t value of intersection
  float t = -b - glm::sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero
  if (t < 0.0f)
    t = 0.0f;
  glm::vec3 q = ray.origin_ + t * ray.dir_;

  // norma calc
  norm = glm::normalize(q - pos_);

  return glm::length(t * ray.dir_);
}

void Sphere::bounding_box()
{
  bbox = Bbox(Eigen::Vector3f(pos_.x - radius, pos_.y - radius, pos_.z - radius),
              Eigen::Vector3f(pos_.x + radius, pos_.y + radius, pos_.z + radius));
}

Box::Box(float x, float y, float z, float vx, float vy, float vz, int id)
  : Shape("Box" + std::to_string(id), {x, y, z}), diagonal(vx, vy, vz)
{
  initBox(x, y, z, vx, vy, vz);
}

Box::Box(glm::vec3 minV, glm::vec3 maxV, int id)
  : Shape("Box" + std::to_string(id), {minV.x, minV.y, minV.z}), diagonal(maxV.x, maxV.y, maxV.z)
{
  initBox(minV.x, minV.y, minV.z, maxV.x, maxV.y, maxV.z);
}

void Box::initBox(float x, float y, float z, float vx, float vy, float vz)
{
  // Front Away Left Right Top Bottom
  points = {
    pos_,                                   // 000  AL  B  0
    {pos_.x, pos_.y + vy, pos_.z},          // 010  A R B  1
    {pos_.x + vx, pos_.y, pos_.z},          // 100 F L  B  2
    {pos_.x + vx, pos_.y + vy, pos_.z},     // 110 F  R B  3
    {pos_.x, pos_.y, pos_.z + vz},          // 001  AL T   4
    {pos_.x, pos_.y + vy, pos_.z + vz},     // 011  A RT   5
    {pos_.x + vx, pos_.y, pos_.z + vz},     // 101 F L T   6
    {pos_.x + vx, pos_.y + vy, pos_.z + vz} // 111 F  RT   7
  };

  normals = {glm::normalize(-glm::cross(points[2] - points[0], points[1] - points[0])),  // Bottom
             glm::normalize(-glm::cross(points[4] - points[6], points[7] - points[6])),  // Top
             glm::normalize(-glm::cross(points[1] - points[0], points[4] - points[0])),  // Away
             glm::normalize(-glm::cross(points[6] - points[2], points[3] - points[2])),  // Front
             glm::normalize(-glm::cross(points[4] - points[0], points[2] - points[0])),  // Left
             glm::normalize(-glm::cross(points[3] - points[1], points[5] - points[1]))}; // Right
  bounding_box();
}

float Box::intersect(const Ray& ray, glm::vec3& normal, bool debug)
{
  float shortest = INFINITY;
  int   i        = 0;

  std::vector<int> pointIndices = {0, 4, 0, 2, 0, 1};

  for (const auto& norm : normals)
  {
    glm::vec3 collision;
    if (planeIntersection(ray, norm, points[pointIndices[i++]], collision))
    {
      if (isInBox(collision))
      {
        float dist = glm::length(collision - ray.origin_);
        if (dist < shortest)
        {
          shortest = std::min(dist, shortest);
          normal   = norm;
        }
      }
    }
  }

  return shortest == INFINITY ? NO_COLLISION : shortest;
}

void Box::bounding_box()
{
  vec3 minPoint = vec3Min(pos_, pos_ + diagonal);
  vec3 maxPoint = vec3Max(pos_, pos_ + diagonal);
  bbox          = Bbox(VecToEigen(minPoint), VecToEigen(maxPoint));
}

bool Box::isInBox(const glm::vec3& point)
{
  bool xmin = point.x <= std::max(pos_.x + diagonal.x, pos_.x) + EPSILON;
  bool ymin = point.y <= std::max(pos_.y + diagonal.y, pos_.y) + EPSILON;
  bool zmin = point.z <= std::max(pos_.z + diagonal.z, pos_.z) + EPSILON;
  bool xmax = point.x >= std::min(pos_.x + diagonal.x, pos_.x) - EPSILON;
  bool ymax = point.y >= std::min(pos_.y + diagonal.y, pos_.y) - EPSILON;
  bool zmax = point.z >= std::min(pos_.z + diagonal.z, pos_.z) - EPSILON;

  return xmin && ymin && zmin && xmax && ymax && zmax;
}

struct Slab
{
  Eigen::Vector3f _N;
  float           _d0, _d1;

  Slab() = default;
  Slab(Eigen::Vector3f N, float d0, float d1) : _N(N), _d0(d0), _d1(d1)
  {
    _N.normalize();
  };
};

class Interval
{
public:
  Interval()
  {
    t0 = 0;
    t1 = INFINITY;
  }

  Interval(float t, float tt, Eigen::Vector3f n, Eigen::Vector3f nn)
  {
    if (t < tt)
    {
      t0      = t;
      t1      = tt;
      normal0 = n;
      normal1 = nn;
    }
    else
    {
      t0      = tt;
      t1      = t;
      normal0 = nn;
      normal1 = n;
    }
  }

  void empty()
  {
    t0      = 0;
    t1      = -1;
    normal0 = Eigen::Vector3f::Zero();
    normal1 = Eigen::Vector3f::Zero();
  }

  bool isEmpty()const
  {
    return (t1 == -1 && t0 == 0);
  }

#define INTERSECTION_EPSILON 0.000001
  void intersect(const Ray& ray, const Slab& slab)
  {
    auto rayDir  = VecToEigen(ray.dir_);
    auto rayOrig = VecToEigen(ray.origin_);
    // forms interval by intersecting ray with slab and intersects with this.
    if (slab._N.dot(rayDir) < INTERSECTION_EPSILON ||
        slab._N.dot(rayDir) > INTERSECTION_EPSILON) // both planes intersected.
    {
      float t0 = -(slab._d0 + slab._N.dot(rayOrig)) / slab._N.dot(rayDir);
      float t1 = -(slab._d1 + slab._N.dot(rayOrig)) / slab._N.dot(rayDir);

      if (t0 < INTERSECTION_EPSILON && t1 < INTERSECTION_EPSILON)
      {
        // this is an empty interval
        empty();
        return;
      }

      t0      = t0 < t1 ? t0 : t1;
      t1      = t0 < t1 ? t1 : t0;
      normal0 = t0 < t1 ? -slab._N : slab._N;
      normal1 = t0 < t1 ? slab._N : -slab._N;

      return;
    }
    else // ray is parallel to the slab
    {
      float sign0 = slab._N.dot(rayOrig) + slab._d0;
      float sign1 = slab._N.dot(rayOrig) + slab._d1;

      if (sign0 > 0 && sign1 < 0 || sign0 < 0 && sign1 > 0) // intersects indefinently
      {
        t0      = 0.0f;
        t1      = INFINITY;
        normal0 = slab._N;
        normal1 = slab._N;
        return;
      }
    }

    // this is an empty interval
    empty();
  }
  
  void intersect(Interval const& other)
  {
    if (other.isEmpty())
    {
      return;
    }

    float tt0 = t0;
    float tt1 = t1;

    t0 = std::max(t0, other.t0);
    t1 = std::min(t1, other.t1);

    if (tt0 != t0)
      normal0 = other.normal0;
    if (tt1 != t1)
      normal1 = other.normal1;
  }
  float           t0, t1;
  Eigen::Vector3f normal0, normal1;
};


float Cylinder::intersect(const Ray& ray, glm::vec3& norm, bool debug)
{
  auto        _A = VecToEigen(axis);
  Quaternionf q  = Quaternionf::FromTwoVectors(_A, Eigen::Vector3f::UnitZ());
  auto        QQ = q._transformVector(VecToEigen(ray.origin_ - pos_)); // base qx, qy, qz
  auto        DD = q._transformVector(VecToEigen(ray.dir_));

  Interval aInterval;
  Slab     slab = Slab(Eigen::Vector3f(0, 0, 1), 0, -_A.norm());
  float a = DD[0] * DD[0] + DD[1] * DD[1];
  float b = 2 * (DD[0] * QQ[0] + DD[1] * QQ[1]);
  float c = QQ[0] * QQ[0] + QQ[1] * QQ[1] - radius * radius;

  float descriminate = b * b - 4.0f * a * c;
  if ((descriminate) < EPSILON)
  {
    return NO_COLLISION;
  }

  float t0 = (-b - sqrt(descriminate)) / (2 * a);
  float t1 = (-b + sqrt(descriminate)) / (2 * a);

  Eigen::Vector3f M0 = QQ + t0 * DD;
  Eigen::Vector3f M1 = QQ + t1 * DD;
  Interval cInterval;
  cInterval.t0      = t0;
  cInterval.t1      = t1;
  cInterval.normal0 = Eigen::Vector3f(M0[0], M0[1], 0.0f).normalized();
  cInterval.normal1 = Eigen::Vector3f(M1[0], M1[1], 0.0f).normalized();

  aInterval.intersect(Ray(EigenToVec(QQ), EigenToVec(DD)), slab);
  aInterval.intersect(cInterval);

  if (aInterval.t0 < EPSILON)
  {
    return NO_COLLISION;
  }

  auto  hitPos = ray.origin_ + aInterval.t0 * ray.dir_;
  float dist   = glm::length(axis);
  if (glm::distance(hitPos, pos_) > dist || glm::dot(glm::normalize(axis),(hitPos - pos_)) < EPSILON)
  {
    return NO_COLLISION;
  }

  if (aInterval.t0 < aInterval.t1)
  {
    norm = EigenToVec(q.conjugate()._transformVector(aInterval.normal0).normalized());
    return aInterval.t0;
  }
  return NO_COLLISION;
}

void Cylinder::bounding_box()
{
  vec3 minPoint = vec3Min(pos_, pos_ + axis);
  vec3 maxPoint = vec3Max(pos_, pos_ + axis);
  bbox          = Bbox(VecToEigen(minPoint), VecToEigen(maxPoint));
}

bool Cylinder::cramers(const std::vector<glm::vec3>& equations, glm::vec3& solution)
{
  glm::mat3x3 D  = {equations[0], equations[1], equations[2]};
  D              = glm::transpose(D);
  glm::mat3x3 Dx = {equations[0], equations[1], equations[2]};
  Dx             = glm::transpose(Dx);
  Dx[0][0]       = equations[3][0];
  Dx[1][0]       = equations[3][1];
  Dx[2][0]       = equations[3][2];
  glm::mat3x3 Dy = {equations[0], equations[1], equations[2]};
  Dy             = glm::transpose(Dy);
  Dy[0][1]       = equations[3][0];
  Dy[1][1]       = equations[3][1];
  Dy[2][1]       = equations[3][2];
  glm::mat3x3 Dz = {equations[0], equations[1], equations[2]};
  Dz             = glm::transpose(Dz);
  Dz[0][2]       = equations[3][0];
  Dz[1][2]       = equations[3][1];
  Dz[2][2]       = equations[3][2];

  float d  = glm::determinant(D);
  float dx = glm::determinant(Dx);
  float dy = glm::determinant(Dy);
  float dz = glm::determinant(Dz);

  if (d != 0)
  {
    solution = glm::vec3(dx / d, dy / d, dz / d);
    return true;
  }
  return false;
}

void Cylinder::printMat3(const glm::mat3x3& mat)
{
  for (int x = 0; x < 3; x++)
  {
    for (int y = 0; y < 3; y++)
    {
      printf("%5.1f ", mat[x][y]);
    }
    printf("\n");
  }
  printf("\n");
}

Model::Model(std::string name, MeshData* data, const vector<Shape*>& shapes, int id)
  : Shape(name + std::to_string(id)), meshData(data)
{
  bounding_box();
  makeBoundingHierarchy(shapes);
}

Model::~Model()
{
  for (auto& box : hierarchy)
  {
    delete box;
    box = nullptr;
  }
}

float Model::intersect(const Ray& ray, glm::vec3& norm, bool debug)
{
  Minimizer minimizer(ray);
  float     minDist = Eigen::BVMinimize(Tree, minimizer);
  return minDist;
}

void Model::bounding_box()
{
  vec3 minPoint(INF, INF, INF);
  vec3 maxPoint(-INF, -INF, -INF);
  for (const auto& vert : meshData->vertices)
  {
    minPoint = vec3Min(minPoint, vert.pnt);
    maxPoint = vec3Max(maxPoint, vert.pnt);
  }
  bbox = Bbox(VecToEigen(minPoint), VecToEigen(maxPoint));
}

void Model::makeBoundingHierarchy(const vector<Shape*>& shapes)
{
  Tree = Eigen::KdBVH<float, 3, Shape*>(shapes.begin(), shapes.end());
}

glm::vec3 Model::centroidOfTri(const vec3& tri)
{
#if DEBUG
  PrintVec3("centroid-v0", meshData->vertices[tri[0]].pnt);
  PrintVec3("centroid-v1", meshData->vertices[tri[1]].pnt);
  PrintVec3("centroid-v2", meshData->vertices[tri[2]].pnt);
  PrintVec3("centroid = ",
            (meshData->vertices[tri[0]].pnt + meshData->vertices[tri[1]].pnt + meshData->vertices[tri[2]].pnt) / 3.0f);
#endif
  return (meshData->vertices[tri[0]].pnt + meshData->vertices[tri[1]].pnt + meshData->vertices[tri[2]].pnt) / 3.0f;
}

std::pair<glm::vec3, glm::vec3> Model::boundsOfTri(const glm::vec3& tri)
{
  auto mnx =
    min(min(meshData->vertices[tri[0]].pnt.x, meshData->vertices[tri[1]].pnt.x), meshData->vertices[tri[2]].pnt.x);
  auto mny =
    min(min(meshData->vertices[tri[0]].pnt.y, meshData->vertices[tri[1]].pnt.y), meshData->vertices[tri[2]].pnt.y);
  auto mnz =
    min(min(meshData->vertices[tri[0]].pnt.z, meshData->vertices[tri[1]].pnt.z), meshData->vertices[tri[2]].pnt.z);
  auto mxx =
    max(max(meshData->vertices[tri[0]].pnt.x, meshData->vertices[tri[1]].pnt.x), meshData->vertices[tri[2]].pnt.x);
  auto mxy =
    max(max(meshData->vertices[tri[0]].pnt.y, meshData->vertices[tri[1]].pnt.y), meshData->vertices[tri[2]].pnt.y);
  auto mxz =
    max(max(meshData->vertices[tri[0]].pnt.z, meshData->vertices[tri[1]].pnt.z), meshData->vertices[tri[2]].pnt.z);
  return {{mnx, mny, mnz}, {mxx, mxy, mxz}};
}

std::pair<glm::vec3, glm::vec3> Model::boundsOfTri(const glm::vec3& tri, const glm::vec3& tri1, const glm::vec3& tri2)
{
  auto bound0 = boundsOfTri(tri);
  auto bound1 = tri1;
  auto bound2 = tri2;
  auto mnx    = min(min(bound0.first.x, bound1.x), bound2.x);
  auto mny    = min(min(bound0.first.y, bound1.y), bound2.y);
  auto mnz    = min(min(bound0.first.z, bound1.z), bound2.z);
  auto mxx    = max(max(bound0.second.x, bound1.x), bound2.x);
  auto mxy    = max(max(bound0.second.y, bound1.y), bound2.y);
  auto mxz    = max(max(bound0.second.z, bound1.z), bound2.z);
  return {{mnx, mny, mnz}, {mxx, mxy, mxz}};
}

bool Model::isInTriangle(int triNumber, const Ray& ray, bool debug)
{
  auto v0 = meshData->vertices[meshData->triangles[triNumber][0]].pnt;
  auto v1 = meshData->vertices[meshData->triangles[triNumber][1]].pnt;
  auto v2 = meshData->vertices[meshData->triangles[triNumber][2]].pnt;
  auto e1 = v1 - v0;
  auto e2 = v2 - v0;
  auto p  = glm::cross(ray.dir_, e2);
  auto d  = glm::dot(p, e1);
  if (d == 0)
    return false;

  auto S = ray.origin_ - v0;
  auto u = glm::dot(p, S) / d;
  if (u < 0 || u > 1)
    return false;

  auto q = glm::cross(S, e1);
  auto v = glm::dot(ray.dir_, q) / d;
  if (v < 0 || (u + v) > 1)
    return false;

  auto t = glm::dot(e2, q) / d;
  if (t < 0)
    return false;

  return true;
}

float Model::Minimizer::minimumOnObject(Shape* obj)
{
  glm::vec3 norm;
  float     dist = obj->intersect(ray, norm);
  if (dist != Shape::NO_COLLISION)
    return dist;
  else
    return INF;
}
// Called by BVMinimize to intersect the ray with a Bbox and
// returns the t value. This should be similar to the already
// written box (3 slab) intersection. (The difference being: a
// distance of zero should be returned if the ray starts within the Bbox.)
// Return INF to indicate no intersection.
float Model::Minimizer::minimumOnVolume(const Bbox& box)
{
  Eigen::Vector3f L = box.min(); // Box corner
  Eigen::Vector3f U = box.max(); // Box corner

  vector<vec3> norms = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  vec3         col;
  float        dist = INF;
  for (const auto& norm : norms)
  {
    if (planeIntersection(ray, norm, EigenToVec(L), col))
    {
      if (Shape::isInBounds(col, EigenToVec(L), EigenToVec(U)))
      {
        dist = std::min(dist, glm::distance(ray.origin_, col));
      }
    }
  }

  return dist;
}
