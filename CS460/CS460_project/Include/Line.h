#pragma once
#ifndef LINE_H
#define LINE_H

#include "Object.h"

class Line : public Object
{
public:
  Line(std::string name);

  void draw();
  void update();

  glm::mat4x4 startMat;
  glm::mat4x4 endMat;

private:
  friend class ObjectManager; // only Object manager should set the ID
};


#endif

