#include "Shape.h"


/**************************** Shape ******************************************************/

Shape::Shape(const string& ShapeName, Vec3 position) : name(ShapeName), pos(position)
{
}

inline bool Shape::planeIntersection(const Ray& ray, const Vec3& norm, const Vec3 planarPoint, Vec3& collisionPoint)
{
  Vec3  normal   = norm.normalized();
  float t        = (planarPoint - ray.origin).dot(normal) / ray.dir.dot(normal);
  collisionPoint = ray.origin + t * ray.dir;

  return t > 0;
}

/**************************** Sphere ******************************************************/

float Sphere::intersect(const Ray& ray, Intersection& inter)
{
  inter.invalid();
  Vec3  m = ray.origin - pos;
  float b = m.dot(ray.dir);
  float c = m.dot(m) - radius * radius;

  // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
  if (c > 0.0f && b > 0.0f)
    return NO_COLLISION;

  float discr = b * b - c;

  // A negative discriminant corresponds to ray missing sphere
  if (discr < 0.0f)
  {
    return NO_COLLISION;
  }

  // Ray now found to intersect sphere, compute smallest t value of intersection
  float t = -b - sqrt(discr);

  // If t is negative, ray started inside sphere so clamp t to zero
  if (t < 0.0f)
    t = 0.0f;
  Vec3 q = ray.origin + t * ray.dir;

  // norma calc
  Vec3 normal = (q - pos).normalized();

  inter.set(t, this, normal, q);

  return (t * ray.dir).norm();
}

void Sphere::bounding_box()
{
  bbox = Bbox(Eigen::Vector3f(pos.x() - radius, pos.y() - radius, pos.z() - radius),
              Eigen::Vector3f(pos.x() + radius, pos.y() + radius, pos.z() + radius));
}

/**************************** Box ******************************************************/

Box::Box(float x, float y, float z, float vx, float vy, float vz, int id)
  : Shape("Box" + std::to_string(id), {x, y, z}), diagonal(vx, vy, vz)
{
  initBox(x, y, z, vx, vy, vz);
}

Box::Box(Vec3 minV, Vec3 maxV, int id) : Shape("Box" + std::to_string(id), minV), diagonal(maxV)
{
  initBox(minV.x(), minV.y(), minV.z(), maxV.x(), maxV.y(), maxV.z());
}

void Box::initBox(float x, float y, float z, float vx, float vy, float vz)
{
  // Front Away Left Right Top Bottom
  points = {
    pos,                                       // 000  AL  B  0
    {pos.x(), pos.y() + vy, pos.z()},          // 010  A R B  1
    {pos.x() + vx, pos.y(), pos.z()},          // 100 F L  B  2
    {pos.x() + vx, pos.y() + vy, pos.z()},     // 110 F  R B  3
    {pos.x(), pos.y(), pos.z() + vz},          // 001  AL T   4
    {pos.x(), pos.y() + vy, pos.z() + vz},     // 011  A RT   5
    {pos.x() + vx, pos.y(), pos.z() + vz},     // 101 F L T   6
    {pos.x() + vx, pos.y() + vy, pos.z() + vz} // 111 F  RT   7
  };

  normals = {((points[2] - points[0]).cross(points[1] - points[0])).normalized(),  // Bottom
             ((points[4] - points[6]).cross(points[7] - points[6])).normalized(),  // Top
             ((points[1] - points[0]).cross(points[4] - points[0])).normalized(),  // Away
             ((points[6] - points[2]).cross(points[3] - points[2])).normalized(),  // Front
             ((points[4] - points[0]).cross(points[2] - points[0])).normalized(),  // Left
             ((points[3] - points[1]).cross(points[5] - points[1])).normalized()}; // Right
  bounding_box();
}

float Box::intersect(const Ray& ray, Intersection& inter)
{
  inter.invalid();
  float shortest = INFINITY;
  int   i        = 0;

  std::vector<int> pointIndices = {0, 4, 0, 2, 0, 1};

  Vec3 normal;
  for (const auto& norm : normals)
  {
    Vec3 collision;
    if (planeIntersection(ray, norm, points[pointIndices[i++]], collision))
    {
      if (isInBox(collision))
      {
        float dist = (collision - ray.origin).norm();
        if (dist < shortest)
        {
          shortest = dist;
          inter.set(dist, this, -norm, ray.origin + dist * ray.dir);
        }
      }
    }
  }

  return shortest == INFINITY ? NO_COLLISION : shortest;
}

void Box::bounding_box()
{
  bbox = Bbox(pos, pos + diagonal);
}

bool Box::isInBox(const Vec3& point)
{
  bool xmin = point.x() <= std::max(pos.x() + diagonal.x(), pos.x()) + EPSILON;
  bool ymin = point.y() <= std::max(pos.y() + diagonal.y(), pos.y()) + EPSILON;
  bool zmin = point.z() <= std::max(pos.z() + diagonal.z(), pos.z()) + EPSILON;
  bool xmax = point.x() >= std::min(pos.x() + diagonal.x(), pos.x()) - EPSILON;
  bool ymax = point.y() >= std::min(pos.y() + diagonal.y(), pos.y()) - EPSILON;
  bool zmax = point.z() >= std::min(pos.z() + diagonal.z(), pos.z()) - EPSILON;

  return xmin && ymin && zmin && xmax && ymax && zmax;
}

/**************************** Cylinder ******************************************************/

float Cylinder::intersect(const Ray& ray, Intersection& inter)
{
  inter.invalid();
  Vec3         v3 = ray.dir.cross(axis);
  vector<Vec3> eq = {ray.dir, -axis, v3, this->pos - ray.origin};
  Vec3         t;
  if (cramers(eq, t))
  {
    // it must fall on the axis length defining the cylinder from origin to the tip of the axis
    if (t.y() > 1 || t.y() < 0)
      return NO_COLLISION;

    Vec3  p3      = ray.origin + t.x() * ray.dir;
    Vec3  p4      = pos + t.y() * axis;
    float Vec3Mag = (p4 - p3).norm();

    // we are interested in the distance from the camera to the edge of the cylinder
    Vec3  v5           = ray.origin - p3;
    float t5           = sqrt(pow(radius, 2) - pow((p4 - p3).norm(), 2)) / v5.norm();
    Vec3  p5           = p3 + t5 * v5;
    float minDistToRay = (p4 - p3).norm();

    if (minDistToRay > radius + EPSILON)
      return NO_COLLISION;

    Vec3 norm = (p5 - p4).normalized();
    inter.set((p5 - ray.origin).norm(), this, norm, ray.origin + minDistToRay * ray.dir);

    return inter.t;
  }

  return NO_COLLISION;
}

void Cylinder::bounding_box()
{
  bbox = Bbox(pos, pos + axis);
}

bool Cylinder::cramers(const std::vector<Vec3>& equations, Vec3& solution)
{
  Mat3 D;
  D << equations[0], equations[1], equations[2];
  D.transpose();
  Mat3 Dx   = D;
  Mat3 Dy   = D;
  Mat3 Dz   = D;
  Dx.col(0) = equations[3];
  Dy.col(1) = equations[3];
  Dz.col(2) = equations[3];

  float d = D.determinant();
  if (d != 0)
  {
    float dx = Dx.determinant();
    float dy = Dy.determinant();
    float dz = Dz.determinant();

    solution = Vec3(dx / d, dy / d, dz / d);
    return true;
  }
  return false;
}
