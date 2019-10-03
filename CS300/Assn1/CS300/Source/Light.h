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

class Light
{
public:
    Light(int shaderProgram = -1, std::string ID = "anon");

    void update();
    glm::vec3 getPosition();
    void translate(glm::vec3 translation);

    Object emitter;
    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    float ambientStrength = 0.5f;

private:
    glm::mat4 transform;
    int colorLoc = -1;  // shader loaction of color 
    int posLoc = -1;    // shader location of position
    int strengthLoc = -1;
};

#endif
