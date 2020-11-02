#include "..\Include\Curve.h"
#include "ObjectManager.h"
#include "Line.h"
using glm::vec3;
using std::vector;
#include <algorithm>
using std::min;
using std::max;

Curve::Curve(const std::vector<glm::vec3>& controlPoints)
{
  setControlPoints(controlPoints);
}

Curve::Curve()
{
  int dimension_ = 3;
  t_min = dimension_;
  t_max = knot.size() - dimension_ - 1;
}

glm::vec3 Curve::evaluate(float t)
{
  if (t >= t_max - 0.01 || t < t_min)
    throw "Bad t value";

  last_t = t;

  return deBoor(dimension_, int(t), t);
}

glm::vec3 Curve::tangent(float t)
{
  float min = dimension_;
  float max = knot.size() - dimension_ - 1.01f;
  float percentage = (t - min) / (max - min);
  if(percentage == 1)
    percentage -= 0.01;

  auto start = evaluate(t);
  float endT = t + 0.01f >= float(knots() - dimension_ - 1) ? 0.01f + dimension_ + (t - (int(t))) : t + 0.01f;
  auto end = evaluate(endT);
  auto tangent = end - start;

  return glm::normalize(tangent);
}

float Curve::getRotation(float t)
{
  auto slope = tangent(t);
  if(slope.x == 0)
  {
    return slope.z > 0 ? 90.0f : 270.0f;
  }

  float degrees = glm::degrees(glm::atan(-slope.z , slope.x));

  return degrees;
}

void Curve::setControlPoints(const std::vector<glm::vec3>& controlPoints)
{
  auto objectMgr = ObjectManager::getObjectManager();
  controlPoints_ = controlPoints;

  // make all the control point balls
  for (int cp = 0; cp < controlPoints_.size(); cp++)
  {
    Object* controlPoint = objectMgr->addObject("control Point");
    controlPoint->loadSphere(1, 50);
    controlPoint->setScale(vec3(0.1));
    controlPoint->setPosition(controlPoints_[cp]);
    controlPoint->material.diffuse = vec3(0, 1, 0);
    controlPoint->material.ambient = vec3(0, 1, 0);
  }

  generatePoints(100);
  for (int i = 0; i < positions_.size(); i++)
  {
    Object* lineObj = objectMgr->addLine("cp-Line");
    Line* line = dynamic_cast<Line*>(lineObj);

    // control debug lines
    line->setStart(glm::translate(glm::mat4x4(1.0f), positions_[i]));
    line->setEnd(glm::translate(glm::inverse(line->getStart()), positions_[(i + 1) % positions_.size()]));
    lineObj->material.diffuse = glm::vec3(1);
    lineObj->material.ambient = glm::vec3(1);
  }

}

float Curve::adjustToNearestT(glm::vec3 pos, float d)
{
  std::map<float,float> G;

  float step = 1.0f / (1 << 10);

  float t = last_t;
  float dist = 0;
  float s0;
  float s1;

  while (dist < d)
  {
    if((t + step) >= (t_max - 0.1f))
    {
      t= t_min;
    }
    s0 = t;
    s1 = t + step;
    t = s1;
    auto p0 = evaluate(s0);
    auto p1 = evaluate(s1);
    G[s1] = G[s0] + glm::distance(p1, p0); 
    dist = glm::distance(pos, p1);
  }

  float a = (d - G[s0]) / (G[s1] - G[s0]);
  float s = s0 + a * (s1 - s0);
  auto final = evaluate(s);
  auto dis = glm::distance(final, pos);
  auto dif = dis/ d;

  return s;
}


void Curve::generatePoints(int pointCount)
{
  positions_.reserve(pointCount);
  for(int i = 0; i < pointCount; i++)
  {
    positions_.push_back(vec3(0));
  }

  int i = 0;
  int N = knot.size() - 1 - dimension_;
  float step = float(N - dimension_) / positions_.size();

  for(int J = dimension_; J < N; J++)
  {
    for (float t = knot[J]; t < knot[J + 1] && i < positions_.size(); i++, t += step)
    {
      vec3 point = deBoor(dimension_, J, t);
      positions_[i] = point;
    }
  }
}

vec3 Curve::deBoor(int k, int i, float t)
{
  if (k == 0 || i == 0)
  {
    return controlPoints_[i];
  }
  else
  {
    float c0 = (knot[i + dimension_ - (k - 1)] - t) / (knot[i + dimension_ - (k - 1)] - knot[i]);
    float c1 = (t - knot[i]) / (knot[i + dimension_ - (k - 1)] - knot[i]);
    return c0 * deBoor(k - 1, i - 1, t) + c1 * deBoor(k - 1, i, t);
  }
}

