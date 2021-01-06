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

  glm::mat4x4 getStart() {return startMat;};
  glm::mat4x4 getEnd() {return endMat;};

  void setStart(const glm::mat4x4& start) {startMat = start; isDirty_= true;};
  void setEnd(const glm::mat4x4& end) {endMat = end; isDirty_ = true;};

private:
  friend class ObjectManager; // only Object manager should set the ID
  bool isDirty_ = false;
  glm::mat4x4 startMat;
  glm::mat4x4 endMat;
};


#endif

