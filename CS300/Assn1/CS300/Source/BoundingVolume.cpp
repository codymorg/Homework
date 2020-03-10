#include "BoundingVolume.h"
#include "ObjectManager.h"
using glm::vec3;
using std::string;

#include <cstdlib>
using std::rand;
#include <algorithm>
#include <vector>
using std::vector;
#include <iostream>
using std::cout;

typedef class Vertex Vertex;

/////***** AABB *****/////

void printVec3( string name,vec3& vec)
{
    printf("%s : %f\t%f\t%f\n",name.c_str(), vec.x, vec.y, vec.z);
}
float maxVec3(vec3& vec)
{
  return std::max(vec.x, std::max(vec.y, vec.z));
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

  if (isSphere_)
  {
    loadSphere(0.5f, 25);
    vec3 maxScale(glm::sqrt(halfScale_.x * halfScale_.x + halfScale_.y * halfScale_.y + halfScale_.z * halfScale_.z));
    halfScale_ = vec3(maxVec3(maxScale));
    this->scale(halfScale_);
    printVec3("scale", halfScale_);
  }
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
    loadSphere(1.0f, 25);
    vec3 maxScale(glm::sqrt(halfScale_.x * halfScale_.x + halfScale_.y * halfScale_.y + halfScale_.z * halfScale_.z));
    halfScale_ = vec3(maxVec3(maxScale));
    this->scale(maxScale);
    printVec3("scale", halfScale_);
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
  //printVec3("min  ", min);
  //printVec3("max  ", max);
  center_ = (max + min) / 2.0f;
  halfScale_ = max - center_;
  //printVec3("center", center_);
  //printVec3("scale ", halfScale_); printf("\n");

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
  //printVec3("min  ", min);
  //printVec3("max  ", max);
  center_ = (max + min) / 2.0f;
  halfScale_ = max - center_;
  this->resetTransform();
  if (isSphere_)
    halfScale_ = vec3(maxVec3(halfScale_));
  this->scale(halfScale_);
  this->translate(center_);
  //printVec3("center", center_);
  //printVec3("scale", halfScale_); printf("\n");
}


void AABB::drawLevel(int onlyThisLevel)
{
  Object* obj = dynamic_cast<Object*>(this);
  AABB* lefty = dynamic_cast<AABB*>(left);
  AABB* righty = dynamic_cast<AABB*>(right);

  // set my draw bool
  obj->drawMe = onlyThisLevel == level;

  // no kids = no recursion
  if (!lefty)
    return;

  lefty->drawLevel(onlyThisLevel);
  righty->drawLevel(onlyThisLevel);
}

void AABB::setColor(int level)
{
  vec3 color;

  if (level % 3 == 0)
  {
    color.r += 0.5;
  }
  else if (level % 2 == 0)
  {
    color.g += 0.5;
  }
  else
  {
    color.b += 0.5;
  }

  if (level >= 4)
  {
    color.r = 1;
  }

  reinterpret_cast<Object*>(this)->material.ambient = color;
  reinterpret_cast<Object*>(this)->material.diffuse = color;
}

// top down
bool AABB::split(int level)
{
  setColor(level);
  if (sortedX.size() > vertexMax)
  {
    cout << level << "\n";
    // determine cut direction based on median data
    int cutYZ = halfScale_.x > halfScale_.y&& halfScale_.x > halfScale_.z; // vertical
    int cutXZ = halfScale_.y > halfScale_.x&& halfScale_.y > halfScale_.z; // horizontal
    int cutXY = halfScale_.z > halfScale_.x&& halfScale_.z > halfScale_.y; // depth cut
    int cutDirection = (cutYZ + (cutXZ << 1) + (cutXY << 2));
    int index = (sortedX.size() / 2);
    int end = sortedX.size();
    //printf("size: %i running:\n  0 - %i\n  %i - %i\n", sortedX.size(), index-1, index, end-1);

    // make sure my children know who i am
    string number = std::to_string(level);
    this->level = level;
    AABB* root = dynamic_cast<AABB*>(this);
    while (root->parent)
    {
      AABB* test = dynamic_cast<AABB*>(root->parent);
      if (!test)
        break;
      else
        root = dynamic_cast<AABB*>(root->parent);
    }
    root->maxLevel = std::max(level, root->maxLevel);
    Object* leftObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_L_") + number);
    Object* rightObj = ObjectManager::getObjectManager()->addVolume<AABB>(this, string("AABB_R_") + number);

    // upkeep and settings
    leftObj->wiremode = true;
    rightObj->wiremode = true;

    // adopt my children
    this->left = dynamic_cast<AABB*>(leftObj);
    this->right = dynamic_cast<AABB*>(rightObj);
    //printf("cut direction: %i\n", cutDirection);
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

    cout << " left ";
    left->split(level + 1);
    cout << " right ";
    right->split(level + 1);

    return true; // there's more splitting that can be done
  }
  else
    cout << '\n';

  return false; // job's done
}


float SquareDistance(vec3 a, vec3 b)
{
  vec3 temp = b - a;
  return (temp.x * temp.x) + (temp.y * temp.y) + (temp.z * temp.z);
}


/////***** Centroid *****/////

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

Centroid::Centroid(Object* parent, std::string name) : BoundingVolume(parent, name)
{
  // calculate center
  findCenter();

  // calculate extent
  enclose();

  // ball draw
  translate(center_);
  scale(halfScale_);
  loadSphere(1, 25); // x == y == z

}

void Centroid::findCenter()
{
  // add up all model space points
  for (auto vert : parent->getVertices())
  {
    center_ += vert.position;
  }

  // model space centroid
  center_ /= parent->getVertices().size();

  // centroid world space
  center_ = parent->modelToWorld(center_);
}

void Centroid::enclose()
{
  // find farthest point from our weighted center
  vec3 maxV(INT_MIN);
  float maxD = INT_MIN;
  for (auto vert : parent->getVertices())
  {
    // these points are in model space - transform them before finding distance
    vec3 world = parent->modelToWorld(vert.position);
    float dist = SquareDistance(world, center_);
    if (dist > maxD)
    {
      maxV = vert.position;
      maxD = dist;
    }
  }

  halfScale_ = vec3(glm::sqrt(maxD));
}

const vec3& Centroid::getCenter()
{
  return center_;
}


/////***** Ritter *****/////


Ritter::Ritter(Object* parent, std::string name) : BoundingVolume(parent, name)
{
  // calculate center
  findCenter();


  // ball draw
  translate(center_);
  scale(halfScale_);
  loadSphere(1, 25); // x == y == z
}

void Ritter::findCenter()
{
  // pick a point
  vec3 pointa = parent->sortedX[sortedX.size() / 2]->position;
  printVec3("reiter a", pointa);

  // find greatest sq dist in model space
  float maxD = 0;
  vec3 pointb(INT_MIN);

  for (auto point : parent->getVertices())
  {
    float dist = SquareDistance(point.position, pointa);
    if (dist > maxD)
    {
      maxD = dist;
      pointb = point.position;
    }
  }
  printVec3("reiter b", pointb);

  // find the fartheest point from that
  maxD = 0;
  vec3 pointc(INT_MIN);

  for (auto point : parent->getVertices())
  {
    float dist = SquareDistance(point.position, pointb);
    if (dist > maxD)
    {
      maxD = dist;
      pointc = point.position;
    }
  }
  printVec3("reiter c", pointc);

  // are all points enclosed?
  maxD = 0;
  vec3 pointd(INT_MIN);
  center_ = (pointc + pointb) / 2.0f;

  for (auto point : parent->getVertices())
  {
    float dist = SquareDistance(point.position, center_);
    if (dist > maxD)
    {
      maxD = dist;
      pointd = point.position;
    }
  }
  printVec3("reiter a", pointd);

  // center of the two as world coords
  printVec3("reiter center", center_);
  center_ = (parent->modelToWorld(pointc) + parent->modelToWorld(pointb)) / 2.0f;


  // radius is from center to one of those points
  vec3 world = parent->modelToWorld(pointd);
  vec3 scale (world - center_);
  halfScale_ = vec3(std::max(std::abs(scale.x), std::max(std::abs(scale.y), std::abs(scale.z))) * 1.1f);

}

Ellipsoid::Ellipsoid(Object* parent, std::string name) : BoundingVolume(parent, name)
{
  // calculate center
  findCenter();

  // ball draw
  translate(center_);
  scale(halfScale_);
  loadSphere(1, 25); // x != y == z
}

void Ellipsoid::findCenter()
{
  // find aabb center
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
  center_ = (max + min) / 2.0f;

  // find farthest point
  float maxD = 0;
  vec3 maxV;
  for (auto point : parent->getVertices())
  {
    float dist = SquareDistance(point.position, vec3(minVert + maxVert) / 2.0f);
    if(dist > maxD)
    {
      maxD = dist;
      maxV = point.position;
    }
  }
  maxV = vec3(parent->sortedX.back()->position.x, parent->sortedY.back()->position.y, parent->sortedZ.back()->position.x);
  // scale is non uniform so keep it this wayu
  vec3 world = parent->modelToWorld(maxV);
  halfScale_ = (world - center_);
  halfScale_.x = abs(halfScale_.x) + glm::sqrt(2) / 4;
  halfScale_.y = abs(halfScale_.y) + glm::sqrt(2) / 4;
  halfScale_.z = abs(halfScale_.z) + glm::sqrt(2) / 4;
}
