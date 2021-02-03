#pragma once
#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/vec3.hpp>

class Camera
{
public:
  Camera(){};

  void setCamera(float px, float py, float pz, float frust, float rw, float rx, float ry, float rz)
  {
    pos_          = {px, py, pz};
    frustumRatio_ = frust;
    rot_          = {rw, rx, ry, rz};
  }

  glm::vec3 pos_;
  glm::quat rot_;
  float     frustumRatio_ = 0;

private:
};
