#include "Quaternion.h"
  using glm::vec4;
  using std::string;
#include <ctime>;
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <limits>

  static string Mat4x4ToString(const glm::mat4& mat)
  {
    string str;
    for (auto i = 0; i < mat.length(); i++)
    {
      for (auto j = 0; j < mat[i].length(); j++)
      {
        str += std::to_string(mat[i][j]) + " ";
        if (j % 3 == 0)
        {
          str += "\n";
        }
      }
    }

    return str;
  }

  static string QuatToString(const glm::quat& quat)
  {
    return "[" + std::to_string(quat.x) + ", " + std::to_string(quat.y) + ", " + std::to_string(quat.z) + ", " + std::to_string(quat.w) + "]";
  }

  static void Verify(const glm::quat& master, const Quaternion& servant, int test, int line, int run)
  {
    float epsilon = 0.01f;
    auto vec = servant.getVector();

    for(auto i = 0; i < master.length(); i++)
    {
      if(glm::abs(master[i] - vec[i]) > epsilon)
      {
        std::string error = "Failed to validate Quaternion Test #";
        std::string masterSt = std::to_string(master.x) + ", " + std::to_string(master.y) + ", " + std::to_string(master.z) + ", " + std::to_string(master.w) + " [x, y, z, w]";
        std::string servantSt = std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ", " + std::to_string(vec.w) + " [x, y, z, w]";
        error += std::to_string(test) + " Line# " + std::to_string(line) + " Run# " + std::to_string(run) + "\nmaster: " + masterSt + "\nservant: " + servantSt;

        std::cout << error;

        throw -1;
      }
    }
  }

  static void Verify(const vec4& master, const vec4& servant, int test, int line, int run)
  {
    float epsilon = 0.01f;

    for (auto i = 0; i < master.length(); i++)
    {
      if (glm::abs(master[i] - servant[i]) > epsilon)
      {
        std::string error = "Failed to validate Quaternion Test #";
        std::string masterSt = std::to_string(master.x) + ", " + std::to_string(master.y) + ", " + std::to_string(master.z) + ", " + std::to_string(master.w) + " [x, y, z, w]";
        std::string servantSt = std::to_string(servant.x) + ", " + std::to_string(servant.y) + ", " + std::to_string(servant.z) + ", " + std::to_string(servant.w) + " [x, y, z, w]";
        error += std::to_string(test) + " Line# " + std::to_string(line) + " Run# " + std::to_string(run) + "\nmaster: " + masterSt + "\nservant: " + servantSt;

        std::cout << error;

        throw - 1;
      }
    }
  }

  static void Verify(float master, float servant, int test, int line, int run)
  {
    float epsilon = 0.01f;
    if (glm::abs(master - servant) > epsilon)
    {
      std::string error = "Failed to validate Quaternion Test #";
      std::string masterSt = "master: " + std::to_string(master);
      std::string servantSt = "servant: " + std::to_string(master);
      error += std::to_string(test) + " Line# " + std::to_string(line) + " Run# " + std::to_string(run) + "\nmaster: " + masterSt + "\nservant: " + servantSt;

      std::cout << error;

      throw - 1;
    }
  }

  static void Verify(const glm::mat4& master, const glm::mat4& servant, int test, int line, int run)
  {
    float epsilon = 0.01f;

    for (auto i = 0; i < master.length(); i++)
    {
      for(auto j = 0; j < master[i].length(); j++)
      {
        if (glm::abs(master[i][j] - servant[i][j]) > epsilon)
        {
          std::string error = "Failed to validate Quaternion Test #";
          error += std::to_string(test) + " Line# " + std::to_string(line) + " Run# " + std::to_string(run) + "\nmaster: " + Mat4x4ToString(master) + "\nservant: " + Mat4x4ToString(servant);
          std::cout << error;

          throw - 1;
        }
      }
    }
  }


  void QuaternionTest(int count)
  {
    srand((unsigned)time(0));

    int runs = count;
    glm::vec3 euler((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
    glm::quat master(euler);
    Quaternion servant(euler.z, euler.y, euler.x);
    int test = 0;

    while(count--)
    {
      /////***** Fuzz testing *****/////
      test = 0;
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      servant = Quaternion(servant.getVector());
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = master + master;
      servant = servant.add(servant);
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = master + master;
      servant = servant.add(servant.getVector());
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      float r = (float)rand() / RAND_MAX * 10;
      master = master * r;
      servant = servant.multiply(r);
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master *= master;
      servant = servant.multiply(servant);
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      float masterF = glm::dot(master, master);
      float servantF = servant.dot(servant);
      Verify(masterF, servantF, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);
    
      masterF = glm::length(master);
      servantF = servant.length();
      Verify(masterF, servantF, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      masterF *= masterF;
      servantF = servant.sqLength();
      Verify(masterF, servantF, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = glm::conjugate(master);
      servant = servant.conjugate();
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = glm::inverse(master);
      servant = servant.inverse();
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      glm::mat4 masterMat = glm::mat4_cast(master);
      glm::mat4 servantMat = servant.getRotationMat();
      Verify(masterMat, servantMat, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      vec4 masterV(master.x, master.y, master.z, master.w);
      vec4 servantV = servant.getVector();
      Verify(masterV, servantV, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = glm::normalize(master);
      servant = servant.normalize();
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      master = -master;
      servant = servant.negate();
      Verify(master, servant, test++, __LINE__, runs - count);
      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      master = glm::quat(euler);
      servant = Quaternion(euler.z, euler.y, euler.x);

      euler = glm::vec3((float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f, (float)rand() / RAND_MAX * 360.0f);
      glm::quat randMaster = glm::quat(euler);
      Quaternion randServant = Quaternion(euler.z, euler.y, euler.x);
      r = (float)rand() / RAND_MAX;
      master = glm::slerp(master, randMaster ,r);
      servant = servant.slerp(randServant.normalize(), r);
      Verify(master, servant, test++, __LINE__, runs - count);

      /////***** Edge Testing *****/////

      master = glm::slerp(master, master, r);
      servant = servant.slerp(servant.normalize(), r);
      Verify(master, servant, test++, __LINE__, runs - count);

      r = std::numeric_limits<float>::min();
      master = glm::slerp(master, randMaster, r);
      servant = servant.slerp(randServant, r);
      Verify(master, servant, test++, __LINE__, runs - count);
    }
    std::cout << "Quaternion passed all " << test << " tests " << runs << " times\n";
  } 


Quaternion::Quaternion(float yaw, float pitch, float roll)
{
  // Abbreviations for the various angular functions
  double cy = cos(yaw * 0.5);
  double sy = sin(yaw * 0.5);
  double cp = cos(pitch * 0.5);
  double sp = sin(pitch * 0.5);
  double cr = cos(roll * 0.5);
  double sr = sin(roll * 0.5);

  q_.w = cr * cp * cy + sr * sp * sy;
  q_.x = sr * cp * cy - cr * sp * sy;
  q_.y = cr * sp * cy + sr * cp * sy;
  q_.z = cr * cp * sy - sr * sp * cy;
}

Quaternion::Quaternion(const vec4& v) 
{
  q_ = v;
}

Quaternion::Quaternion(const aiQuaternion& quat)
{
  q_.w = quat.w;
  q_.x = quat.x;
  q_.y = quat.y;
  q_.z = quat.z;
}

Quaternion Quaternion::add(const Quaternion& q) const
{
  vec4 vector = q.getVector();
  return Quaternion(vector + q_);
}

Quaternion Quaternion::add(const vec4& v) const
{
  return Quaternion(q_ + v);
}

Quaternion Quaternion::multiply(float scalar) const
{
  return Quaternion(q_ * scalar);
}

Quaternion Quaternion::multiply(const Quaternion& q) const
{
  vec4 vec = q.getVector();
  float r0 = q_.w;
  float r1 = q_.x;
  float r2 = q_.y;
  float r3 = q_.z;
  float q0 = vec.w;
  float q1 = vec.x;
  float q2 = vec.y;
  float q3 = vec.z;
  float t0 = r0 * q0 - r1 * q1 - r2 * q2 - r3 * q3;
  float t1 = r0 * q1 + r1 * q0 - r2 * q3 + r3 * q2;
  float t2 = r0 * q2 + r1 * q3 + r2 * q0 - r3 * q1;
  float t3 = r0 * q3 - r1 * q2 + r2 * q1 + r3 * q0;

  return Quaternion(vec4(t1, t2, t3, t0));
}

float Quaternion::dot(const Quaternion& q) const
{
  vec4 p = q.getVector();

  return p.w * q_.w + p.x * q_.x + p.y * q_.y + p.z * q_.z;
}

float Quaternion::length() const
{
  return glm::sqrt(sqLength());
}

float Quaternion::sqLength() const
{
  return q_.w * q_.w + q_.x * q_.x + q_.y * q_.y + q_.z * q_.z;
}

Quaternion Quaternion::conjugate() const
{
  auto vec = getVector();
  return Quaternion(vec4(-vec.x, -vec.y, -vec.z, vec.w));
}

Quaternion Quaternion::inverse() const
{
  return Quaternion(conjugate().getVector() / sqLength());
}

glm::mat4x4 Quaternion::getRotationMat() const
{
  return glm::mat4x4(
    1 - 2 * q_.y * q_.y - 2 * q_.z * q_.z,  2 * q_.x * q_.y + 2 * q_.w * q_.z,     2 * q_.x * q_.z - 2 * q_.w * q_.y,        0,
    2 * q_.x * q_.y - 2 * q_.w * q_.z,      1 - 2 * q_.x * q_.x - 2 * q_.z * q_.z, 2 * q_.y * q_.z + 2 * q_.w * q_.x,        0,
    2 * q_.x * q_.z + 2 * q_.w * q_.y,      2 * q_.y * q_.z - 2 * q_.w * q_.x,     1 - 2 * q_.x * q_.x - 2 * q_.y * q_.y,    0,
    0,                                      0,                                     0,                                        1);
}

vec4 Quaternion::getVector() const
{
  return q_;
}

Quaternion Quaternion::negate() const
{
  return Quaternion(vec4(-q_.x, -q_.y, -q_.z, -q_.w));
}

Quaternion Quaternion::normalize() const
{
  auto vec = getVector();
  vec /= length();
  return Quaternion(vec);
}

// referencing Xin Li's notes
Quaternion Quaternion::slerp(const Quaternion& end, float percent) const
{
  Quaternion unitStart = normalize();
  Quaternion unitEnd = end.normalize();

  double dotp = unitStart.dot(unitEnd);
  bool needsNegate = false;

  if (dotp < 0.0f)
  {
    needsNegate = true;
    unitStart = unitStart.negate();
    dotp = -dotp;
  }

  // don't bother with slerp if the angle is too close - lerp is good enough
  if (dotp > 0.995f)
  {
    auto ret = unitStart.add(unitEnd.add(unitStart.negate()).multiply(percent)).normalize();
    if(needsNegate)
      ret = ret.negate();

    return ret;
  }

  float alpha = std::acos(dotp);

  auto ret = (unitStart.multiply(std::sin(alpha - percent * alpha)).add(unitEnd.multiply(std::sin(percent * alpha))).multiply(1.0f / std::sin(alpha)));
  if(needsNegate)
    ret = ret.negate();

  return ret;
}

