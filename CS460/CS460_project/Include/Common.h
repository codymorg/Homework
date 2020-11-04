/******************************************************************************
  Project : Create and manage different scenes
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#ifndef Common_H
#define Common_H
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include "assimp/matrix4x4.h"

glm::mat4x4 ConvertaiMatToglm(const aiMatrix4x4& mat);
glm::vec3 ConvertaiVecToGlm(const aiVector3D ai);
void PrintMat(const glm::mat4x4& mat, std::string inden = "");

template <class vec>
void PrintVec(vec v, int newlines = 0, std::string label ="")
{
  std::cout << label <<"[ ";
  for(auto i = 0; i < v.length(); i++)
  {
    std::cout << v[i] << " ";
  }
  std::cout << " ]";
  while(newlines-- > 0)
    std::cout << "\n";
}


#endif
