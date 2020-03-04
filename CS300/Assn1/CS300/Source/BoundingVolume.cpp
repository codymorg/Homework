#include "BoundingVolume.h"
#include "ObjectManager.h"
using glm::vec3;
using std::string;

#include <algorithm>
#include <vector>
using std::vector;

typedef class Vertex Vertex;

/////***** AABB *****/////


AABB::AABB(Object* object, string name) : BoundingVolume(object, name)
{
  findCenter();
  enclose();
  this->loadBox(halfScale_);
  this->translate(center_);

  // copy sorted 
  this->sortedX = object->sortedX;
  this->sortedY = object->sortedY;
  this->sortedZ = object->sortedZ;
}

void AABB::findCenter()
{
  // AABB uses unweighted center
  center_ = (parent->getMinWorldPos() + parent->getMaxWorldPos()) / 2.0f;
}

// find the center of children boxes
void AABB::recalculateBounds(vector<Vertex*>& sorted, int minIndex, int maxIndex)
{
  
  // find min / max model space
  vec3 maxVert = vec3(INT_MIN);
  vec3 minVert = vec3(INT_MAX);
  this->sortedX.clear();
  this->sortedY.clear();
  this->sortedZ.clear();
  printf("limitedbounds:\n\n");
  for (int i = minIndex; i < maxIndex; i++)
  {
    Vertex& vert = *sorted[i];

    // build our sorted containers
    this->sortedX.push_back(sorted[i]);
    this->sortedY.push_back(sorted[i]);
    this->sortedZ.push_back(sorted[i]);
    printf("%i: %f\t%f\t%f\n", i, vert.position.x, vert.position.y, vert.position.z);
    // get extrema
    maxVert.x = std::max(maxVert.x, vert.position.x);
    maxVert.y = std::max(maxVert.y, vert.position.y);
    maxVert.z = std::max(maxVert.z, vert.position.z);

    minVert.x = std::min(minVert.x, vert.position.x);
    minVert.y = std::min(minVert.y, vert.position.y);
    minVert.z = std::min(minVert.z, vert.position.z);
  }

  // sort our containers
  sort(sortedX.begin(), sortedX.end(), [&](Vertex* l, Vertex* r) {return l->position.x < r->position.x; });
  sort(sortedY.begin(), sortedY.end(), [&](Vertex* l, Vertex* r) {return l->position.y < r->position.y; });
  sort(sortedZ.begin(), sortedZ.end(), [&](Vertex* l, Vertex* r) {return l->position.z < r->position.z; });


  // move the box into the right spot for drawing
  vec3 oldCenter = center_;               // world space
  vec3 temp = (maxVert + minVert) / 2.0f; // model space
  center_ = parent->modelToWorld(temp);      // world space
  halfScale_ = parent->modelToWorld(maxVert) - center_; // world space
  translate(-oldCenter);
  scale(halfScale_);
  translate(center_);
}

void AABB::enclose()
{
  halfScale_ = vec3(parent->getMaxWorldPos() - center_);
}

static char number = '0';
// top down
bool AABB::split()
{
  // determine cut direction based on median data
  int cutYZ = halfScale_.x > halfScale_.y && halfScale_.x > halfScale_.z; // vertical
  int cutXZ = halfScale_.y > halfScale_.x && halfScale_.y > halfScale_.z; // horizontal
  int cutXY = halfScale_.z > halfScale_.x && halfScale_.z > halfScale_.y; // depth cut
  int cutDirection = (cutYZ + (cutXZ << 1) + (cutXY << 2));
  int index = (sortedX.size() / 2);
  int end = sortedX.size();

  // make sure my children know who i am
  Object* leftObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_L_") + number);
  Object* rightObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_R_") + number);

  // upkeep and settings
  leftObj->wiremode = true;
  rightObj->wiremode = true;
  number++;

  // adopt my children
  this->left = dynamic_cast<AABB*>(leftObj);
  this->right = dynamic_cast<AABB*>(rightObj);
  printf("cut direction: %i\n",cutDirection);
  switch (cutDirection)
  {
    // they are all equal - just cut along YZ arbitrarily
  case 0:
  case 1:
    dynamic_cast<AABB*>(leftObj)->recalculateBounds(sortedX, 0, index);
    dynamic_cast<AABB*>(rightObj)->recalculateBounds(sortedX, index, end);
    break;

  case 2:
    dynamic_cast<AABB*>(leftObj)->recalculateBounds(sortedY, 0, index);
    dynamic_cast<AABB*>(rightObj)->recalculateBounds(sortedY, index, end);
    break;

  case 4:
    dynamic_cast<AABB*>(leftObj)->recalculateBounds(sortedZ, 0, index);
    dynamic_cast<AABB*>(rightObj)->recalculateBounds(sortedZ, index, end);
    break;

    // no other combos possible
  default:
    break;
  }


  return true;
}

// bot up

/////***** Sphere *****/////

BoundingVolume::BoundingVolume(Object* object, string name) : Object(name)
{
  this->parent = object;
}
