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
#include "ShaderManagement.h"

class Camera
{
public:
  Camera(glm::vec3 position, float angle, glm::vec3 axis, unsigned shader);

  void update(ShaderManager& shaderManager);
  void translate(glm::vec3 translation);
  void reset();

  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection;

  int width = 0;
  int height = 0;
private:
  glm::vec3 initialPos;
  glm::vec3 position;


  unsigned shaderProgram = 0;
  int viewLoc;         // view matrix shader location
  int projectionLoc;   // projection matrix shader locaiton
  int cameraPosLoc;    // load where the camera is for lighting
};

#endif

