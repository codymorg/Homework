/******************************************************************************
  Brief   : manages and contains all information about the 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "ShaderManager.h"

typedef class Camera;

class Vertex
{
public:
  Vertex(glm::vec3 pos);
  Vertex(glm::vec3 pos, glm::vec3 norm);

  glm::vec3 position;
  glm::vec3 normal;
  // UV's will be generated by the GPU
};

class Object
{
  public:
    Object(std::string ID = "anon");
    virtual ~Object();

    // object shape
    void loadOBJ(std::string location);
    void loadeCube(float radius);
    void loadSphere(float radius, int divisions);
    void loadCircle(float radius, int divisions, glm::vec3 axis = glm::vec3(0,1,0));
    void loadPlane(float radius);
    void loadTexture(std::string location, int slot = 0);

    // object manipulation
    void translate(glm::vec3 trans);
    void rotate(float degrees, glm::vec3 center = glm::vec3(0), glm::vec3 axis = glm::vec3(0,1,0));
    void scale(glm::vec3 scale);
    void draw();

    // Getters
    unsigned  getShaderProgram();
    glm::vec3 getWorldPosition();

    // Setters
    void setShader(ShaderType type);

    // object attributes
    void genFaceNormals();
    void genVertexNormals();

    // public data
    std::string name;
    bool        wiremode = false;
    bool        debugObject = false;
    struct MaterialData
    {
      glm::vec3 ambient = glm::vec3(1);
      float     padding_I = 0.0f;
      glm::vec3 diffuse = glm::vec3(1);
      float     padding_II = 0.0f;
      glm::vec3 specular = glm::vec3(1);
      float     padding_III = 0.0f;
    }material;

  private:
    
    // object data
    glm::vec3           boundingBox_[2];
    glm::mat4           preTransform_ = glm::mat4(1.0f); // used mostly for rotating 
    glm::mat4           modelToWorld_ = glm::mat4(1.0f);
    unsigned            renderMode_ = GL_TRIANGLES;
    std::vector<Vertex> vertices_ = 
    {
      Vertex(glm::vec3(-0.5,-0.5,0)),
      Vertex(glm::vec3(0.5,-0.5,0)),
      Vertex(glm::vec3(0,0.5,0)),
    };
    std::vector<unsigned int> indices_ = { 0,1,2 };
    std::vector<glm::vec3>faceNormals_;

    // openGL stuff
    Shader shader_;
    unsigned vbo_ = 0;
    unsigned vao_ = 0;
    unsigned ebo_ = 0;
    unsigned fbo_ = 0;
    unsigned rbo_ = 0;

    // object functions
    void initBuffers();

    // uniform locations
    int textureCountLoc_ = -1;
    int modelToWorldLoc_ = -1;
    int boundingBoxLoc_ = -1;
};





#endif