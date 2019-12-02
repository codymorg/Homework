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

Camera::Camera(vec3 position,float angle, vec3 axis, unsigned shader) : shaderProgram(shader)
{
  view = glm::rotate(view, glm::radians(angle), axis);
  view = glm::translate(view, position);
  this->position = position;
  initialPos = position;
  projection = glm::perspective(glm::radians(45.0f), 4 / 3.0f, 0.1f, 1000.0f);
 
  // get the shader locations for these matrices
  glUseProgram(shaderProgram);
  viewLoc = glGetUniformLocation(shaderProgram, "view");
  projectionLoc = glGetUniformLocation(shaderProgram, "projection");
  cameraPosLoc =  glGetUniformLocation(shaderProgram, "cameraPos");
  assert(viewLoc >= 0);
}

void Camera::translate(glm::vec3 translation)
{
  view = glm::translate(view, translation);
  position += translation;
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

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    //glUniform3fv(cameraPosLoc, 1, glm::value_ptr(view[3]));
    glUseProgram(0);

  }
}

