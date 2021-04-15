#pragma once
#include "common.h"
class Camera
{
public:
  Camera(){};

  void setCamera(float px, float py, float pz, float frust, float rw, float rx, float ry, float rz)
  {
    pos           = {px, py, pz};
    frustumRatio = frust;
    rot           = {rw, rx, ry, rz};
  }

  Vec3  pos;
  Quat  rot;
  float frustumRatio = 0;

private:
};
