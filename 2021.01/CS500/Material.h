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
  void setRGB(float r, float g, float b)
  {
    rgba = {r, g, b, 0};
  }
  float alpha() const 
  {
    return rgba.a;
  }
  bool      isLight = false;
  glm::vec3 specular;

private:
  glm::vec4 rgba    = {1, 1, 1, 1};
};
