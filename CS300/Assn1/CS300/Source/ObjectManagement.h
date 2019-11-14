/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Instructions on how to use this software
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_1
Author  :   Cody Morgan  ID: 180001017
Date  :   4 OCT 2019
End Header --------------------------------------------------------*/


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
  Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv = glm::vec2(0));

  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

struct Line
{
  Line(glm::vec3 pointA, glm::vec3 pointB);

  glm::vec3 centroid;
  glm::vec3 normDir;
};

struct MaterialData
{
  glm::vec3 ambient = glm::vec3(1);
  float padding_I = 0.123f;
  glm::vec3 diffuse = glm::vec3(1);
  float padding_II = 0.123f;
  glm::vec3 specular = glm::vec3(1);
  float padding_III = 0.123f;
};

class Texture
{
public:

  enum class Projector
  {
    Cube,
    Sphere,
    Cylindrical
  };
  
  Texture(std::string location, Projector projector = Projector::Sphere);
  ~Texture();
  void changeTexture(std::string location, Projector projector = Projector::Sphere);
  void bindTbo();
  glm::vec2 generateUV(glm::vec3 boundingBoxLower, glm::vec3 boundingBoxUpper, glm::vec3 point);

private:

  Projector projector_;
  unsigned char* buffer_;
  unsigned tbo_ = 0;
  int width_;
  int height_;
  int channels_;
  std::string location_;

  void initBuffer();

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
  void loadPlane();
  void loadTexture(std::string location, Texture::Projector projector = Texture::Projector::Sphere);

  // manipulation
  void translate(glm::vec3 translation);
  void spin(float degrees, glm::vec3 axis);
  void rotateY(float degrees, float radius);
  void addScale(glm::vec3 scale);

  // face/vertex normal stuff
  void initLineBuffers();
  void genFaceNormals();
  void drawFaceNormals();
  void genVertexNormals();
  void drawVertexNormals();

  // misc functionality
  void draw();
  void printTransform();
  void toggleFaceNormals(bool useHardSet = false, bool setToThis = false);
  void toggleVertexNormals(bool useHardSet = false, bool setToThis = false);
  void setShader(int shaderProgram);


  std::string name;           // name of objectec
  unsigned renderMode = GL_TRIANGLES; // the method to interpret the vertex data
  bool fillPolygons = true;       // wiremode: false
  glm::mat4 transform;
  glm::vec3 minPos;
  glm::vec3 maxPos;
  float orbitRadius;
  float vectorScale = 1 / 50.0f;
  MaterialData material;


  // face normal management
  enum FaceNormalDrawingMode
  {
    InvalidDrawMode = -1,
    FaceNormalsOff,
    FaceNormalsOn,
  };
  enum VertexNormalDrawingMode
  {
    VertexNormalOff = 0,
    VertexNormalOn
  };
  int faceNormDrawingMode = InvalidDrawMode;
  int vertexNormalDrawingMode = InvalidDrawMode;


//private:
  void initBuffers();

  unsigned shaderProgram_;    // compiled shader program
  unsigned vbo;               // vertex buffer
  unsigned vao;               // attributes
  unsigned ebo;               // indices
  int modelLoc = -1;          // shader location of model matrix
  float modelScale = 1.0f;

  std::vector<Vertex> vertices =  // vertex info of object
  {
    Vertex(-0.5f, -0.5f, 0.0f),
    Vertex(0.5f, -0.5f, 0.0f),
    Vertex(0.5f,  0.5f, 0.0f),
    Vertex(-0.5f,  0.5f, 0.0f),
  };

  std::vector<unsigned> indices =
  {
    0, 1, 2,
    0, 2, 3
  };

  std::vector<glm::vec3> faceNorms;
  unsigned lineShader;
  unsigned lineVAO;
  unsigned lineVBO;
  int vectorColorLoc = -1;       // shader location of color
  Texture* texture = nullptr;
};

class MaterialManager
{
public:
  void genUBO(unsigned shaderProgram);

  void updateUBO(MaterialData& material);

private:
  unsigned ubo_ = -1;
};




#endif