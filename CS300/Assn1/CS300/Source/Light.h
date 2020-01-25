/******************************************************************************
  Project : Create and manage different lights
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"
class Light : public Object
{
public:
  Light(std::string name);

  void draw();
  void update();

  // phong data
  struct LightData
  {
    glm::vec4 position = glm::vec4(0);
    glm::vec4 ambient = glm::vec4(10/255.0f);
    glm::vec4 diffuse = glm::vec4(1);
    glm::vec3 specular = glm::vec3(1);
    float ns = 100.0f;
    glm::vec4 emissive = glm::vec4(1);
    glm::vec3 attenuation = glm::vec3(.1);
    int number = 0;
    glm::vec3 direction = glm::vec3(0, 0.1, -1);
    int type = 0;
    glm::vec2 spot = glm::vec2(glm::cos(glm::pi<float>() / 8), glm::cos(glm::pi<float>() / 4));
    float padding[2];
  }lightData;

private:
  unsigned ID = 0;

  friend class ObjectManager; // only Object manager should set the ID
};


#endif

