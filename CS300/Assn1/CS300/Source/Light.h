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
