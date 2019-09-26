/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: Manages Camera
    Date:  4 OCT 2019

******************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H
#pragma once

#include <glm/glm.hpp>


class Camera
{
public:
    Camera(glm::vec3 position, float angle, glm::vec3 axis, unsigned shader);

    void update();

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;
    unsigned shaderProgram = 0;

private:
};

#endif

