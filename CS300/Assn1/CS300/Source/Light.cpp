/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Instructions on how to use this software
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date    :   4 OCT 2019
End Header --------------------------------------------------------*/


#include "Light.h"
using std::string;
using glm::vec3;

Light::Light(int shaderProgram, string ID) : emitter(shaderProgram, ID)
{
    emitter.loadSphere(1,50);
    emitter.color = vec3(1,1,0);
    colorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    posLoc = glGetUniformLocation(shaderProgram, "lightPos");
    strengthLoc = glGetUniformLocation(shaderProgram, "lightStrength");


    //assert(colorLoc >= 0);
    //assert(posLoc >= 0);
    //assert(strengthLoc >= 0);
    //assert(colorOverrideLoc >= 0);


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


void Light::update()
{
  glUniform3fv(colorLoc,1, glm::value_ptr(color));
  glUniform3fv(posLoc, 1, glm::value_ptr(getPosition()));
  glUniform1f(strengthLoc, ambientStrength);

  emitter.draw();
}

