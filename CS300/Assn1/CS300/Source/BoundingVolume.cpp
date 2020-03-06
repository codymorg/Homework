#include "BoundingVolume.h"
#include "ObjectManager.h"
using glm::vec3;
using std::string;

#include <algorithm>
#include <vector>
using std::vector;
#include <iostream>
using std::cout;

typedef class Vertex Vertex;

/////***** AABB *****/////

void printVec3( string name,vec3 vec)
{
    printf("%s : %f\t%f\t%f\n",name.c_str(), vec.x, vec.y, vec.z);
}

AABB::AABB(Object* object, string name) : BoundingVolume(object, name)
{
  // tree properties for the children to enjoy
  AABB* parentBox = dynamic_cast<AABB*>(parent);
  if (parentBox)
  {
    this->vertexMax = parentBox->vertexMax;
    this->isSphere_ = parentBox->isSphere_;
  }

  findCenter();
  enclose();

  if(isSphere_)
    loadSphere(std::max(halfScale_.x, std::max(halfScale_.y, halfScale_.z)), 25);
  else
    this->loadBox(vec3(halfScale_));
  this->translate(center_);

  // copy sorted 
  this->sortedX = object->sortedX;
  this->sortedY = object->sortedY;
  this->sortedZ = object->sortedZ;



}

void AABB::drawAsSphere(bool isSphere)
{
  isSphere_ = isSphere;
  if (isSphere)
  {
    loadSphere(0.5f, 25);
    vec3 maxScale(glm::sqrt(halfScale_.x * halfScale_.x + halfScale_.y * halfScale_.y + halfScale_.z * halfScale_.z));
    halfScale_ = maxScale;
    this->scale(maxScale);
  }
  else
    loadBox(halfScale_);

  AABB* box = dynamic_cast<AABB*>(this);

    printf("sphere %s\n", box->name.c_str());
  if (box->left)
  {
    dynamic_cast<AABB*>(box->left)->drawAsSphere(isSphere);
    dynamic_cast<AABB*>(box->right)->drawAsSphere(isSphere);
  }
}

void AABB::findCenter()
{
  // AABB uses unweighted center
  center_ = (parent->getMinWorldPos() + parent->getMaxWorldPos()) / 2.0f;
  vec3 minVert(INT_MAX);
  vec3 maxVert(INT_MIN);
  for (auto vert : parent->getVertices())
  {
    maxVert.x = std::max(maxVert.x, vert.position.x);
    maxVert.y = std::max(maxVert.y, vert.position.y);
    maxVert.z = std::max(maxVert.z, vert.position.z);

    minVert.x = std::min(minVert.x, vert.position.x);
    minVert.y = std::min(minVert.y, vert.position.y);
    minVert.z = std::min(minVert.z, vert.position.z);
  }
  vec3 min = parent->modelToWorld(minVert);
  vec3 max = parent->modelToWorld(maxVert);
  printVec3("min  ", min);
  printVec3("max  ", max);
  center_ = (max + min) / 2.0f;
  halfScale_ = max - center_;
  printVec3("center", center_);
  printVec3("scale ", halfScale_); printf("\n");

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
  //printf("limitedbounds:\n\n");
  for (int i = minIndex; i < maxIndex; i++)
  {
    Vertex& vert = *sorted[i];

    // build our sorted containers
    this->sortedX.push_back(sorted[i]);
    this->sortedY.push_back(sorted[i]);
    this->sortedZ.push_back(sorted[i]);
    //printf("%i ", i);
    //printVec3("vert", vert.position);
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

  Object* model = parent;
  while (dynamic_cast<AABB*>(model))
    model = dynamic_cast<AABB*>(model)->parent;

  vec3 min = model->modelToWorld(minVert);
  vec3 max = model->modelToWorld(maxVert);
  printVec3("min  ", min);
  printVec3("max  ", max);
  center_ = (max + min) / 2.0f;
  halfScale_ = max - center_;
  this->resetTransform();
  this->scale(halfScale_);
  this->translate(center_);
  printVec3("center", center_);
  printVec3("scale", halfScale_); printf("\n");
}

void AABB::enclose()
{
  //vec3 max = parent->getMaxWorldPos();
  //halfScale_ = vec3(parent->getMaxWorldPos() - center_);
  
}

// top down
bool AABB::split(int level)
{
  cout << "level: " << level << "\n";

  if (sortedX.size() > vertexMax)
  {
    // determine cut direction based on median data
    int cutYZ = halfScale_.x > halfScale_.y&& halfScale_.x > halfScale_.z; // vertical
    int cutXZ = halfScale_.y > halfScale_.x&& halfScale_.y > halfScale_.z; // horizontal
    int cutXY = halfScale_.z > halfScale_.x&& halfScale_.z > halfScale_.y; // depth cut
    int cutDirection = (cutYZ + (cutXZ << 1) + (cutXY << 2));
    int index = (sortedX.size() / 2);
    int end = sortedX.size();
    printf("size: %i running:\n  0 - %i\n  %i - %i\n", sortedX.size(), index-1, index, end-1);

    // make sure my children know who i am
    string number = std::to_string(level);
    Object* leftObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_L_") + number);
    Object* rightObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_R_") + number);

    // upkeep and settings
    leftObj->wiremode = true;
    rightObj->wiremode = true;

    // adopt my children
    this->left = dynamic_cast<AABB*>(leftObj);
    this->right = dynamic_cast<AABB*>(rightObj);
    printf("cut direction: %i\n", cutDirection);
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

    left->split(level+1);
    right->split(level+1);

    return true; // there's more splitting that can be done
  }

  return false; // job's done
}



// bot up

/////***** Sphere *****/////

BoundingVolume::BoundingVolume(Object* object, string name) : Object(name)
{
  this->parent = object;
}

void BoundingVolume::setTopDownMode(TopDownMode mode)
{
  topDownMode_ = mode;

  if (this->topDownMode_ == TopDownMode::heightMax)
    vertexMax = parent->getVertices().size() / (1 << (heightMax - 1));
}
