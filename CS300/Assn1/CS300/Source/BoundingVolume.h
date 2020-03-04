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
  BoundingVolume(Object* parent, std::string name);

  // public data
  BoundingVolume* right = nullptr;
  BoundingVolume* left = nullptr;
  glm::vec3 color;
  Object* parent = nullptr;

  enum class TopDownMode
  {
    vertexMax,
    heightMax
  };
  TopDownMode topDownMode = TopDownMode::vertexMax;
  int vertexMax = 15; // when using vertex mode
  int heightMax = 7;   // when using height max mode


  // basic bounsing volume functions
  virtual void findCenter() = 0; // find the center of object
  virtual void enclose() = 0;    // determine how to encolse all points in volume

  virtual bool split(int level) = 0;

protected:

};

class AABB : public BoundingVolume
{
public:
  AABB(Object* parent, std::string name);

  // hierarchy funcitons
  bool split(int level);

  // overridden funcitons
  void findCenter();
  void recalculateBounds(std::vector<Vertex*>& sorted, int minIndex, int maxIndex);
  void enclose();

protected:
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

private:
  // volume data
  glm::vec3 center_ = glm::vec3();
  float radius_ = 0;
};

#endif
