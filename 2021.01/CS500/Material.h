#pragma once
#include <glm/glm/vec3.hpp>
#include <glm/glm/vec4.hpp>

class MyMaterial
{
public:
  MyMaterial(){};
  MyMaterial(const glm::vec3& rgb, const glm::vec3& specular, float alpha)
    : rgba(rgb.x, rgb.y, rgb.z, alpha), specular(specular){};

  void setMaterial(float dr, float dg, float db, float sr, float sg, float sb, float a)
  {
    rgba     = {dr, dg, db, a};
    specular = {sr, sg, sb};
  };
  glm::vec3 rgb()
  {
    return {rgba.r, rgba.g, rgba.b};
  };
  bool      isLight = false;
  glm::vec4 rgba    = {1, 1, 1, 1};
  glm::vec3 specular;

private:
};
