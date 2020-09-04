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
#include <assimp/scene.h>


#include <string>
#include <vector>

#include "ShaderManager.h"
typedef class Camera Camera;

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
  void loadModel(std::string location);
  void clearObject();
  void loadFolder(std::string location);
  void loadBox(glm::vec3 halfScale);
  void loadSphere(float radius, int divisions);


  // object manipulation
  void translate(glm::vec3 trans);
  void rotate(float degrees, glm::vec3 center = glm::vec3(0), glm::vec3 axis = glm::vec3(0, 1, 0));
  void scale(glm::vec3 scale);
  glm::vec3 modelToWorld(glm::vec3 point);
  void resetTransform();
  virtual void draw();
  virtual void update();

  // public data
  std::string name;
  bool        wiremode = false;
  unsigned    renderMode = GL_TRIANGLES;
  bool        drawMe = true;

  // Getters
  int       getShaderProgram();
  Shader&   getShader();
  glm::vec3 getWorldPosition();
  glm::vec3 getWorldScale();
  glm::vec3 getMinWorldPos();
  glm::vec3 getMaxWorldPos();
  const std::vector<Vertex>& getVertices();


  // Setters
  void setShader(ShaderType type);

  // object attributes
  void genFaceNormals();
  void genVertexNormals();
  struct MaterialData
  {
    glm::vec3 ambient = glm::vec3(0);
    float     paddingI = 0;
    glm::vec3 diffuse = glm::vec3(1);
    float     paddingII = 0;
    glm::vec3 specular = glm::vec3(1);
    float     paddingIII = 0;
  }material;

  std::vector<Vertex*> sortedX;
  std::vector<Vertex*> sortedY;
  std::vector<Vertex*> sortedZ;
  aiVector3D centroid;

private:
  void updateSorted();

  // object data
  glm::mat4   preTransform_ = glm::mat4(1.0f); // used mostly for rotating 
  glm::mat4   modelToWorld_ = glm::mat4(1.0f);

  // geometry data
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
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

  void processNode(aiNode* node, const aiScene* scene, aiAABB& maxBounding);
  aiAABB processMesh(aiMesh* mesh, const aiScene* scene);
  aiAABB bounds_ = aiAABB(aiVector3D(INT_MAX, INT_MAX, INT_MAX), aiVector3D(INT_MIN, INT_MIN, INT_MIN));
};





#endif