#pragma once
#include "Common.h"

class Curve
{
  public:

  Curve(const std::vector<glm::vec3>& controlPoints);
  Curve();

  glm::vec3 evaluate(float t);
  glm::vec3 tangent(float t);
  float getRotation(float t);
  void setControlPoints(const std::vector<glm::vec3>& controlPoints);
  float adjustToNearestT(glm::vec3 pos, float d);

  int dimensions(){return dimension_;};
  int knots(){return knot.size();};
  float t_min = dimension_;
  float t_max = knot.size() - dimension_ - 1;

  private:
  int dimension_ = 3;
  float last_t;

  std::vector<glm::vec3> positions_;
  std::vector<int>       knot = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
  std::vector<glm::vec3> controlPoints_;

  void generatePoints(int pointCount);
  auto deBoor(int k, int i, float t)->glm::vec3;
};
