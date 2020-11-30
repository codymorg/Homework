#pragma once
#ifndef PARENTOBJ_H
#define PARENTOBJ_H

#include "Object.h"

class Parented : public Object
{
public:
  Parented(std::string name, int segments, float width);

  void draw();
  void update();

  void addAcceleration(glm::vec3 acceleration, int node = -1);
  void addVelocity(glm::vec3 vel, int node = -1);

private:
  std::vector<glm::mat4x4> transforms_;
  std::vector<glm::vec3> velocities_;
  std::vector<glm::vec3> accelerations_;
  std::vector<float> masses_;
  float energyConservation_ = 0.9;
};


#endif

