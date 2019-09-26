/******************************************************************************
    Name : Cody Morgan
    Class: CS 300
    Assn : 01
    Brief: mesh and vertex classes public access
    Date:  4 OCT 2019

******************************************************************************/


#ifndef MESH_H
#define MESH_H
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <istream>

class Vertex 
{
public:
    Vertex(float x, float y, float z);
    glm::vec3 position;
    glm::vec3 color;
};

class Object
{
public:
    Object(unsigned shader, unsigned VAO, unsigned VBO, unsigned EBO);

    void addVertex(float x, float y, float z);
    void draw();

    int ID = -1;
    unsigned shaderProgram;                  // compiled shader program
    unsigned vbo;                            // vertex buffer
    unsigned vao;                            // attributes
    unsigned ebo;                            // indices
    unsigned renderMode = GL_TRIANGLES;      // the method to interpret the vertex data
    unsigned drawFrequency = GL_STATIC_DRAW; // drawing suggestion
    bool fillPolygons = true;                // wiremode: false
    glm::vec4 polyColor;                     // the color of this object
    glm::vec3 transform = glm::vec3(0,0,0);       // position
    float rotation = 0;                      // rotation
    float zSort = 0;                         // sorting order
    int attributeCount = 2;                  // how many components you have [vert(1), normal(2), color(3)]
    glm::vec3 modelScale = glm::vec3(1,1,1);

    vector<float> vertices;
    vector<float> colors;
    vector<unsigned> indices;
};

class Mesh
{
public:

    vector<float> vertices;
    vector<unsigned> indices;

};

Mesh LoadOBJ(std::string fileLocation);

#endif


