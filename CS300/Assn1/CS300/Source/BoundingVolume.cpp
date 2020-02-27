#include "BoundingVolume.h"
using glm::vec3;
using std::string;

/////***** AABB *****/////


AABB::AABB(Object* object, string name) : BoundingVolume(object, name)
{
  findCenter();
  enclose();
  this->loadBox(halfScale_);
}

void AABB::findCenter()
{
  // AABB uses unweighted center
  center_ = (model->getMin() + model->getMax()) / 2.0f;
}

void AABB::enclose()
{
  halfScale_ = vec3(this->getMax() - center_);
}

// top down
bool AABB::split()
{
  return true;
}

// bot up
AABB AABB::join(const BoundingVolume& sibling) const
{
  return *this;
}


/////***** Sphere *****/////

BoundingVolume::BoundingVolume(Object* object, string name) : Object(name)
{
  this->model = object;
}
