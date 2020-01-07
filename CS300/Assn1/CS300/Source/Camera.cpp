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


#include "Camera.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using glm::vec3;

Camera::Camera(vec3 position, float angle, vec3 axis)
{
  worldToCam = glm::rotate(worldToCam, glm::radians(angle), axis);
  worldToCam = glm::translate(worldToCam, position);
  this->position = position;
  initialPos = position;
  rotation = axis * angle;
  projection = glm::perspective(glm::radians(45.0f), 4 / 3.0f, 0.1f, 1000.0f);
}

void Camera::translate(glm::vec3 translation)
{
  worldToCam = glm::translate(worldToCam, translation);
  position += translation;
}

void Camera::rotate(float degrees, vec3 axis)
{
  worldToCam = glm::rotate(worldToCam, glm::radians(degrees), axis);
}

void Camera::reset()
{
  translate(initialPos - position);

}


void Camera::update(ShaderManager& shaderManager)
{
  std::vector<int> shaders = shaderManager.getAllShaders();
  for (int shader : shaders)
  {
    glUseProgram(shader);
    viewLoc = glGetUniformLocation(shader, "view");
    projectionLoc = glGetUniformLocation(shader, "projection");
    //cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    assert(viewLoc >= 0);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &worldToCam[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    //glUniform3fv(cameraPosLoc, 1, glm::value_ptr(view[3]));
    glUseProgram(0);

  }
}

