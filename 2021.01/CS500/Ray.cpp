#include "Ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : origin_(origin)
{
  dir_ = glm::normalize(direction);
  //dir_ = direction;
}
