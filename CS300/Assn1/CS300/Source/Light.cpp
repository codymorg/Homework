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


#include "Light.h"
using std::string;
using glm::vec3;
using glm::vec4;
using std::vector;

static LightManagement lightManagement;

LightManagement* GetLightManager()
{
  return &lightManagement;
}

Light::Light(int shaderProgram, string ID) : emitter(shaderProgram, ID)
{
  emitter.loadSphere(1,50);
  emitter.color = vec3(1,1,0);
  setShader(shaderProgram);

}

vec3 Light::getPosition()
{
  return vec3(transform[3]);
}

void Light::translate(glm::vec3 translation)
{
  emitter.translate(translation);
  transform = glm::translate(transform, translation);
}

void Light::setShader(int shaderProgram)
{
  colorLoc = glGetUniformLocation(shaderProgram, "lightColor");
  posLoc = glGetUniformLocation(shaderProgram, "lightPos");
  strengthLoc = glGetUniformLocation(shaderProgram, "lightStrength");
}

void Light::rotateY(float degrees, float radius)
{
  glm::vec3 pos(transform[3][0], transform[3][1], transform[3][2]);
  if (pos.x > radius)
    transform[3][0] = radius;

  // rotate verts
  glm::mat4 rotM(1.0f);
  rotM = glm::translate(rotM, -pos);
  rotM = glm::rotate(rotM, glm::radians(degrees), vec3(0, 1, 0));
  rotM = glm::translate(rotM, pos);
  transform = rotM * transform;
  emitter.rotateY(degrees, radius);
}

void Light::setColor(glm::vec3 newColor)
{
  color = newColor;
  emitter.color = newColor;
}


void Light::update()
{
  //glUniform3fv(colorLoc,1, glm::value_ptr(color));
  glUniform3fv(posLoc, 1, glm::value_ptr(getPosition()));
  glUniform1f(strengthLoc, ambientStrength);

  emitter.draw();
}

void LightManagement::genUBO(unsigned shaderProgram)
{
  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Light::LightData) * lightMax, nullptr, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManagement::updateUBO(vector<Light>& lights)
{
  void* uboBuffer;
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  uboBuffer = glMapNamedBuffer(ubo, GL_READ_WRITE);

  for (unsigned i = 0; i < lights.size(); i++)
  {
    lights[i].lightData.position = vec4(lights[i].getPosition(), 1);
    memcpy(uboBuffer, &lights[i].lightData, sizeof(lights[i].lightData));
  }

  glUnmapNamedBuffer(ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
