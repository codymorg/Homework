/******************************************************************************
  Project : Bounding volumes including
            AABB, OBB, Sphere, ritter's sphere, pca & elipsoid
                    [BV]
            [AABB]        [Sphere]
            [OBB]         [Ritter] [PCA] [Elipsoid]
  Name    : Cody Morgan
  Date    : 24 FEB 2020
******************************************************************************/

#pragma once
#ifndef BV_H
#define BV_H

#include <string>
#include <vector>

#include "Object.h"

class BoundingVolume : public Object
{
public:
  BoundingVolume(Object* model, std::string name);
  virtual bool split() = 0; // split this bounding volume into halves

  // public data
  BoundingVolume* right = nullptr;
  BoundingVolume* left = nullptr;
  glm::vec3 color;
  Object* model = nullptr;

protected:

  // basic bounsing volume functions
  virtual void findCenter() = 0; // find the center of object
  virtual void enclose() = 0;    // determine how to encolse all points in volume

};

class AABB : public BoundingVolume
{
public:
  AABB(Object* model, std::string name);

  // hierarchy funcitons
  bool split();
  AABB join(const BoundingVolume& sibling) const;

protected:

  // overridden funcitons
  void findCenter();
  void enclose();

  // volume data
  glm::vec3 center_ = glm::vec3();
  glm::vec3 halfScale_; // radius measurment from center
};


class Sphere : public BoundingVolume
{
  Sphere(std::string location);

  // overridden funcitons
  void findCenter();
  void enclose();
  bool split();


  // hierarchy funcitons
  Sphere join(const BoundingVolume& sibling) const;

private:
  // volume data
  glm::vec3 center_ = glm::vec3();
  float radius_ = 0;
};

#endif
