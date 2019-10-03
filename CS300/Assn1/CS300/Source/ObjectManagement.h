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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <istream>
#include <vector>

struct Vertex
{
    Vertex(float x, float y, float z, float nx = 0, float ny = 0, float nz = 0);
    Vertex(glm::vec3 pos, glm::vec3 norm);

    glm::vec3 position;
    glm::vec3 normal;
};

struct Line
{
    Line(glm::vec3 pointA, glm::vec3 pointB);

    glm::vec3 centroid;
    glm::vec3 normDir;
};

class Object
{
public:
    Object(int shaderPgm, std::string ID = "anon");
    ~Object();

    // loading meshes
    void loadOBJ(std::string fileLocation);
    void loadCube();
	void loadSphere(float radius, int divisions);
    void loadcircle(float radius, int divisions);

    // manipulation
    void translate(glm::vec3 translation);
    void spin(float degrees, glm::vec3 axis);
    void rotateY(float degrees, float radius);
    void addScale(glm::vec3 scale);

    // face normal stuff
    void initLineBuffers();
    void genFaceNormals();
    void drawFaceNorms();

    // misc functionality
    void draw();
    void printTransform();
    void toggleFaceNormals();

    std::string name;                           // name of objectec
    unsigned renderMode = GL_TRIANGLES;         // the method to interpret the vertex data
    bool fillPolygons = true;                   // wiremode: false
    glm::mat4 transform;
    glm::vec3 minPos;
    glm::vec3 maxPos;
    float orbitRadius;
    glm::vec3 color = {0, 0.25, 0.5};
    float vectorScale = 1 / 50.0f;

    // face normal management
    enum DrawNormalMode
    {
        InvalidDrawMode = -1,
        FaceNormalsOff,
        FaceNormalsOn
    };
    int faceNormDrawingMode = InvalidDrawMode;


private:
    void initBuffers();

    unsigned shaderProgram;   // compiled shader program
    unsigned vbo;             // vertex buffer
    unsigned vao;             // attributes
    unsigned ebo;             // indices
    int modelLoc;             // shader location of model matrix
    int colorLoc;             // shader location of color
    glm::vec3 scale = {1,1,1};

    std::vector<Vertex> vertices =  // vertex info of object
    {
        Vertex(-0.5f, -0.5f, 0.0f),
        Vertex( 0.5f, -0.5f, 0.0f),
        Vertex( 0.5f,  0.5f, 0.0f),
        Vertex(-0.5f,  0.5f, 0.0f),
    };

    std::vector<unsigned> indices =
    {
        0, 1, 2,
        0, 2, 3
    };

    std::vector<glm::vec3> faceNorms;
    unsigned lineVAO;
    unsigned lineVBO;
};


#endif