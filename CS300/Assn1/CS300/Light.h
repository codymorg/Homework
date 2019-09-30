/******************************************************************************
  Name : Cody Morgan
  Class: CS 300
  Assn : 01
  Brief: Manages Lights
  Date:  4 OCT 2019

******************************************************************************/

#ifndef LIGHT_H
#define LIGHT_H
#pragma once

#include <glm/glm.hpp>
#include "ObjectManagement.h"

#include <string>

class Light : public Object
{
public:
    Light(int shaderProgram = -1, std::string ID = "anon");


private:
    glm::vec3 color;
    glm::mat4 transform;
};

#endif
