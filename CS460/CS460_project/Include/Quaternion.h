#pragma once
#include <ctime>;
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <limits>
#include <string>
#include <assimp/quaternion.h>

void QuaternionTest(int count);


class Quaternion
{
public:
  Quaternion(float yaw, float pitch, float roll);
  Quaternion(const glm::vec4& v);
  Quaternion(const aiQuaternion& quat);

  Quaternion add(const Quaternion& q) const;
  Quaternion add(const glm::vec4& v) const;
  Quaternion multiply(float scalar) const;
  Quaternion multiply(const Quaternion& q) const;
  float      dot(const Quaternion& q) const;
  float      length() const;
  float      sqLength() const;
  Quaternion conjugate() const;
  Quaternion inverse() const;
  Quaternion negate() const;

  glm::mat4x4 getRotationMat() const;
  glm::vec4   getVector() const;
  Quaternion  normalize() const;

  friend std::ostream& operator<<(std::ostream& output, const Quaternion& quat) 
  {
    auto q = quat.getVector();
    output << "{ " << q.w << ", " << q.x << ", " << q.y << ", " << q.z << " }wxyz";
    return output;
  }

  // referencing Xin Li's notes
  Quaternion slerp(const Quaternion& end, float percent) const;

  private:
  glm::vec4 q_;
};

