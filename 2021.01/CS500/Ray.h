#pragma once
#include <glm/glm/vec3.hpp>
#include <glm/glm/geometric.hpp>

class Ray
{
public:
  Ray(const glm::vec3& origin, const glm::vec3& direction);

  glm::vec3 origin_;
  glm::vec3 dir_;
private:
};

