/******************************************************************************
  Project : Create and manage different scenes
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#pragma once
#ifndef Common_H
#define Common_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>


// common vectors
glm::vec3 up(0, 1, 0);
glm::vec3 right(1, 0, 0);
glm::vec3 down(0, -1, 0);
glm::vec3 left(-1, 0, 0);
glm::vec3 back(0, 0, -1);
glm::vec3 forward(0, 0, 1);



#endif