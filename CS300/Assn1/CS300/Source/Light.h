/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Instructions on how to use this software
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date  :   4 OCT 2019
End Header --------------------------------------------------------*/


#ifndef LIGHT_H
#define LIGHT_H
#pragma once

#include "ObjectManagement.h"

#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <string>

class Light
{
public:
  Light(int shaderProgram = -1, int emitterShader = -1, std::string ID = "anon");
  ~Light();

  //***** Light functionality *****//
  void update();
  glm::vec3 getPosition();
  void translate(glm::vec3 translation);
  void setShader(int shaderProgram);
  unsigned getShader();
  void rotateY(float degrees, float radius);
  void setColor(glm::vec3 color);


  //***** Light qualities *****//
  Object emitter;
  glm::vec3 color = {1.0f, 1.0f, 1.0f};
  float ambientStrength = 0.5f;

  struct LightData
  {
    glm::vec4 position = glm::vec4(0);
    glm::vec4 ambient = glm::vec4(.1, .1, .1, 0);
    glm::vec4 diffuse = glm::vec4(1, 1, 1, 0);
    glm::vec3 specular = glm::vec3(1);
    float ns = 100.0f;
    glm::vec4 emissive = glm::vec4(0);
    glm::vec3 attenuation = glm::vec3(1);
    int number = 0;
    glm::vec3 direction = glm::vec3(0,0.1,-1);
    int type = 0;
    glm::vec2 spot = glm::vec2(glm::cos(glm::pi<float>()/8), glm::cos(glm::pi<float>()/4));
    float padding[2];
  }lightData;

private:
  glm::mat4 transform;
  int shaderProgram_ = -1;
};

struct LightManagement
{
  const unsigned lightMax = 16;
  void genUBO(unsigned shaderProgram);

  void updateUBO(std::vector<Light>& lights);

  unsigned ubo = 0;
  unsigned blockIndex = 0;

};

LightManagement* GetLightManager();


#endif
