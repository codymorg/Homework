#include "Line.h"
#include "ObjectManager.h"


Line::Line(std::string name) : Object(name)
{
  loadLine();
}

void Line::draw()
{
  this->Line::update();
  Object::draw();
}
void Line::update()
{
  if (isDirty_)
  {
    isDirty_ = false;

    // set first point
    setTransform(startMat);

    glm::vec3 pos(endMat[3][0], endMat[3][1], endMat[3][2]);

    if(vertices_[1].position != pos)
    {
      vertices_[1].position = pos;
      initBuffers();
    }
  }
}
