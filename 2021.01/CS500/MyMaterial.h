#pragma once
#include "common.h"

class MyMaterial
{
public:
  MyMaterial()
  {
  }

  MyMaterial(const Vec3& rgb, const Vec3& spec, float alph) : diffuse(rgb), specular(spec), alpha(alph)
  {
  }

  bool  isLight = 0;
  Vec3  diffuse;
  float alpha;
  Vec3  specular;
};
