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


#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "ShaderManager.h"

class Camera
{
public:
  Camera(glm::vec3 position, float angle, glm::vec3 axis);

  void translate(glm::vec3 translation);
  void rotate(float degrees, glm::vec3 axis);
  void reset();

  glm::mat4 worldToCam = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);

  int width = 0;
  int height = 0;
  glm::vec3 position;
  glm::vec3 rotation;

private:
  glm::vec3 initialPos;
};
#endif

