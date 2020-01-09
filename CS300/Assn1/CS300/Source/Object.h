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

    // public data
    std::string name;
    bool        wiremode = false;

    // Getters
    unsigned  getShaderProgram();
    glm::vec3 getWorldPosition();

    // Setters
    void setShader(std::string vs, std::string fs, ShaderType type);

    // object attributes
    void genFaceNormals();
    void genVertexNormals();

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

class ObjectManager
{
  public:
    static ObjectManager* getObjectManager();
    ~ObjectManager();

    void    render(Camera& camera);
    void    removeAllObjects();
    Object* addObject(std::string ID = "anon");
    bool    isValid();


    // getters
    std::vector<Object*> getObjectsByName(std::string name);
    Object*              getFirstObjectByName(std::string name);
    Object*              getAt(unsigned index);
    Object*              getSelected();
    unsigned             getSize();

    // data
    int selectedObject = 0; // which object the GUI is looking at

  private:
    ObjectManager(){};
    static ObjectManager* objectManager_;

    std::vector<Object*> objects_;
    bool                isValid_ = false;
};

#endif