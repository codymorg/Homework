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


#include "Camera.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using glm::vec3;

Camera::Camera(vec3 position,float angle, vec3 axis, unsigned shader) : shaderProgram(shader)
{
    view = glm::rotate(view, glm::radians(angle), axis);
    view = glm::translate(view, position);
    projection = glm::perspective(glm::radians(45.0f), 4 / 3.0f, 0.1f, 1000.0f);

    // get the shader locations for these matrices
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    assert(viewLoc >= 0);
    assert(projectionLoc >= 0);
}

void Camera::update()
{
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

