#include "Shape.h"
#include "glm/glm/geometric.hpp"
#include "glm/glm/mat3x3.hpp"
#include "glm/glm/matrix.hpp"
#include <algorithm>
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

float Cylinder::intersect(const Ray& ray, glm::vec3& norm, bool debug)
{
  // L1 = ray = orig + t1*dir              // dir is normalized
  // L2 = Cyl.pos + t2(Cyl.end - Cyl.pos)  // this vector is the length of the cylinder
  // p3 = p1 + t1*v1
  // v3 = v1 X v2
  // p4 = p3 + t3*v3
  // p4 = p2 + t2*v2
  // p3 + t3*v3 = p2 + t2*v2
  // p1 + t1*v1 + t3*v3 = p2 + t2*v2
  // t1*v1 -t2*v2 + t3*v3 = p2 - p1
  // by cramers -> (t1,t2,t3)
  // |V3| < radius
  // 0 <= t2 <= 1

  vec3                   v3 = glm::cross(ray.dir_, axis);
  std::vector<glm::vec3> eq = {ray.dir_, -axis, v3, this->pos_ - ray.origin_};
  vec3                   t;
  if (cramers(eq, t))
  {
    // it must fall on the axis length defining the cylinder from origin to the tip of the axis
    if (t.y > 1 || t.y < 0)
      return NO_COLLISION;

    vec3  p3      = ray.origin_ + t.x * ray.dir_;
    vec3  p4      = pos_ + t.y * axis;
    float vec3Mag = glm::length(p4 - p3);

    // we are interested in the distance from the camera to the edge of the cylinder
    vec3  v5 = ray.origin_ - p3;
    float t5 = glm::sqrt(glm::pow<float>(radius, 2) - glm::pow<float>(glm::length(p4 - p3), 2)) / glm::length(v5);
    vec3  p5 = p3 + t5 * v5;
    float minDistToRay = glm::length(p4 - p3);

    if (minDistToRay > radius + EPSILON)
      return NO_COLLISION;

    norm = glm::normalize(p5 - p4);

    return glm::length(p5 - ray.origin_);
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
        dist =  std::min(dist,glm::distance(ray.origin_, col));
      }
    }
  }

  return dist;
}
