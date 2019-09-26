/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: Manages Camera
    Date:  4 OCT 2019

******************************************************************************/

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
}

void Camera::update()
{
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

