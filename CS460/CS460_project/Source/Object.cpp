/******************************************************************************
  Brief   : manages and contains all information about the 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#include "Camera.h"
#include "ShaderManager.h"
#include "ObjectManager.h"
#include "Object.h"
using std::vector;
using std::string;
using glm::vec3;
using glm::vec4;
using glm::mat4;

#include <fstream>
using std::ifstream;
#include <sstream>
#include <algorithm>
using std::min;
using std::max;
using std::sort;
#include <iostream>
using std::cout;
#include <filesystem> // C++17

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


/////***** Vertex stuff *****/////

Vertex::Vertex(vec3 pos) : position(pos), normal(vec3(0))
{}

Vertex::Vertex(vec3 pos, vec3 norm) : position(pos), normal(norm)
{}


/////***** Object stuff *****/////

Object::Object(std::string ID)
{
  name = ID;
  modelToWorld_ = mat4(1.0f);
  setShader(ShaderType::Deferred);
  // init openGL
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  initBuffers();
}

void Object::processNode(aiNode* node, const aiScene* scene, aiAABB& maxBounding)
{
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

    // find the max bounding box so we can scale our object down to unit size
    aiAABB aabb = processMesh(mesh, scene);
    maxBounding.mMin = std::min(maxBounding.mMin, aabb.mMin);
    maxBounding.mMax = std::max(maxBounding.mMax, aabb.mMax);
  }

  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++)
  {
    processNode(node->mChildren[i], scene, maxBounding);
  }
}

aiAABB Object::processMesh(aiMesh* mesh, const aiScene* scene)
{
  int indexAdjust = vertices_.size();
  for (unsigned int i = 0; i < mesh->mNumVertices; i++)
  {
    aiVector3D pos = mesh->mVertices[i];
    aiVector3D aiNorm = mesh->mVertices[i];
    glm::vec3 norm = vec3(aiNorm.x, aiNorm.y, aiNorm.z);
    if (mesh->HasNormals())
    {
      aiNorm = mesh->mNormals[i];
      norm = vec3(aiNorm.x, aiNorm.y, aiNorm.z);
    }
    
    vertices_.push_back(Vertex(glm::vec3(pos.x, pos.y, pos.z), norm));
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
    {
      indices_.push_back(face.mIndices[j] + indexAdjust);

    }
  }

  return mesh->mAABB;
}

void Object::loadModel(string fileLocation)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(fileLocation, 
    aiProcess_Triangulate | 
    aiProcess_GenSmoothNormals |
    aiProcess_JoinIdenticalVertices |
    aiProcess_OptimizeMeshes |
    aiProcess_GenBoundingBoxes
  );

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
  }
  else
  {
    cout << "loading file:" << fileLocation << "\n" 
         << "meshes: " << scene->mNumMeshes << "\n"
         << "animations: " <<  scene->mNumAnimations << "\n";

    for(auto i = 0; i < scene->mNumAnimations; i++)
    {
      cout << scene->mAnimations[i]->mName.C_Str() << "\n"
           << "duration: " << scene->mAnimations[i]->mDuration << "\n"
           << "num channels: " << scene->mAnimations[i]->mNumChannels << "\n\n";
    }

    vertices_.clear();
    indices_.clear();

    modelToWorld_ = glm::mat4();

    processNode(scene->mRootNode, scene, bounds_);

    // scale down to unit size and put at the origin
    aiVector3D aiModelScale = (bounds_.mMax - bounds_.mMin);
    vec3 modelScale = vec3(aiModelScale.x, aiModelScale.y, aiModelScale.z);
    float maxScale = std::max(std::max(modelScale.x, modelScale.y), modelScale.z);
    maxScale = 4 / maxScale; // unit size of 2

    // move to origin
    centroid = (bounds_.mMin * maxScale + bounds_.mMax * maxScale) / 2.0f;
    translate(-vec3(centroid.x, centroid.y, centroid.z));
    scale(vec3(maxScale));
    initBuffers();
  }
}

void Object::clearObject()
{
  vertices_.clear();
  indices_.clear();
  //resetTransform();
}

void Object::loadBox(vec3 halfScale)
{
  scale(halfScale);
  halfScale = vec3(1.0f);
  this->renderMode = GL_LINE_STRIP;
  vertices_ =
  {
    Vertex(vec3(-halfScale.x, -halfScale.y,  halfScale.z), vec3(-halfScale.x, -halfScale.y,  halfScale.z)),
    Vertex(vec3(halfScale.x, -halfScale.y,  halfScale.z),  vec3(halfScale.x, -halfScale.y,  halfScale.z)),
    Vertex(vec3(halfScale.x,  halfScale.y,  halfScale.z),  vec3(halfScale.x,  halfScale.y,  halfScale.z)),
    Vertex(vec3(-halfScale.x,  halfScale.y,  halfScale.z), vec3(-halfScale.x,  halfScale.y,  halfScale.z)),
    Vertex(vec3(-halfScale.x, -halfScale.y, -halfScale.z), vec3(-halfScale.x, -halfScale.y, -halfScale.z)),
    Vertex(vec3(halfScale.x, -halfScale.y, -halfScale.z),  vec3(halfScale.x, -halfScale.y, -halfScale.z)),
    Vertex(vec3(halfScale.x,  halfScale.y, -halfScale.z),  vec3(halfScale.x,  halfScale.y, -halfScale.z)),
    Vertex(vec3(-halfScale.x,  halfScale.y, -halfScale.z), vec3(-halfScale.x,  halfScale.y, -halfScale.z))
  };

  bounds_.mMax = aiVector3D(halfScale.x, halfScale.y, halfScale.z);
  bounds_.mMin = aiVector3D(-halfScale.x, -halfScale.y, -halfScale.z);

  indices_ =
  {
    //0,1,2, // Front
    //0,2,3,
    //0,5,1, // Bottom
    //0,4,5,
    //1,5,6, // Right
    //1,6,2,
    //4,0,3, // Left
    //4,3,7,
    //4,7,6, // Back
    //4,6,5,
    //3,2,6, // Top
    //3,6,7
    //lines
    0,1,2,3,0, 
    4,7,3,2,   
    6,7,4,     
    5,6,5,     
    1
  };

  initBuffers();


}

void Object::loadSphere(float radius, int divisions)
{
  // reset object geometry
  vertices_.clear();
  indices_.clear();

  bounds_.mMax = aiVector3D(radius, radius, radius);
  bounds_.mMin = aiVector3D(-radius, -radius, -radius);

  vec3 position;
  vec3 norm;
  float pi = glm::pi<float>();
  float sectorStep = 2.0f * pi / divisions;
  float stackStep = pi / divisions;
  float theta = 0;
  float phi = 0;
  float length = 1.0f / radius;

  // generate vertices
  for (int i = 0; i <= divisions; ++i)
  {
    theta = pi / 2 - i * stackStep;
    float xy = radius * cosf(theta);
    position.z = radius * sinf(theta);

    for (int j = 0; j <= divisions; ++j)
    {
      phi = j * sectorStep;
      position.x = xy * cosf(phi);
      position.y = xy * sinf(phi);

      // normalized vertex normal (nx, ny, nz)
      norm.x = position.x / radius;
      norm.y = position.y / radius;
      norm.z = position.z / radius;

      vertices_.push_back(Vertex(position, norm));
    }
  }

  // generate indices
  unsigned k1;
  unsigned k2;
  for (int i = 0; i < divisions; ++i)
  {
    k1 = i * (divisions + 1);   // beginning of current stack
    k2 = k1 + divisions + 1;    // beginning of next stack

    for (int j = 0; j < divisions; ++j, ++k1, ++k2)
    {
      if (i != 0)
      {
        indices_.push_back(k1);
        indices_.push_back(k2);
        indices_.push_back(k1 + 1);
      }

      // k1+1 => k2 => k2+1
      if (i != (divisions - 1))
      {
        indices_.push_back(k1 + 1);
        indices_.push_back(k2);
        indices_.push_back(k2 + 1);
      }
    }
  }

  initBuffers();
}

// world transform  
void Object::translate(glm::vec3 trans)
{
  //modelToWorld_ = glm::translate(modelToWorld_, trans);
  modelToWorld_[3][0] += trans.x;
  modelToWorld_[3][1] += trans.y;
  modelToWorld_[3][2] += trans.z;
}

void Object::rotate(float degrees, glm::vec3 center, glm::vec3 axis)
{
  modelToWorld_ = glm::translate(modelToWorld_, center);
  modelToWorld_ = glm::rotate(modelToWorld_, glm::radians(degrees), axis);
  modelToWorld_ = glm::translate(modelToWorld_, -center);
}

void Object::scale(glm::vec3 scale)
{
  //modelToWorld_ = glm::scale(modelToWorld_, scale);
  modelToWorld_[0][0] = scale.x;
  modelToWorld_[1][1] = scale.y;
  modelToWorld_[2][2] = scale.z;
}

glm::vec3 Object::modelToWorld(vec3 point)
{
  return vec3(modelToWorld_ * glm::vec4(point,1));
}

void Object::resetTransform()
{
  modelToWorld_ = glm::mat4();
}

void Object::update()
{
  // update all uniform variables
  int currentSP = ShaderManager::getShaderManager()->getCurrentBound();

  modelToWorldLoc_ = glGetUniformLocation(currentSP, "modelToWorld");
  if (modelToWorldLoc_ != -1)
    glUniformMatrix4fv(modelToWorldLoc_, 1, GL_FALSE, &(modelToWorld_[0][0]));

  ObjectManager::getObjectManager()->updateUBO(this);
}

// draw this object using its own shader
void Object::draw()
{
  if (drawMe)
  {
    // bind shader and vao
    glUseProgram(shader_.getProgram());
    glBindVertexArray(vao_);

    // draw mode
    if (!wiremode)
      glPolygonMode(GL_FRONT, GL_FILL);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    this->Object::update();

    // draw
    glDrawElements(renderMode, indices_.size(), GL_UNSIGNED_INT, 0);

    // unbind
    glBindVertexArray(0);
    glUseProgram(0);
  }
}


int Object::getShaderProgram()
{
  return shader_.getProgram();
}

Shader& Object::getShader()
{
  return shader_;
}

vec3 Object::getWorldPosition()
{
  return vec3(modelToWorld_[3]);
}

glm::vec3 Object::getWorldScale()
{
  return glm::vec3(modelToWorld_[0][0], modelToWorld_[1][1], modelToWorld_[2][2]);
}

glm::vec3 Object::getMinWorldPos()
{
  return vec3(modelToWorld_ * vec4(bounds_.mMin.x, bounds_.mMin.y, bounds_.mMin.z, 1));
}

glm::vec3 Object::getMaxWorldPos()
{
  return vec3(modelToWorld_* vec4(bounds_.mMax.x, bounds_.mMax.y, bounds_.mMax.z, 1));
}

const std::vector<Vertex>& Object::getVertices()
{
  return vertices_;
}


void Object::setShader(ShaderType type)
{
  shader_ = ShaderManager::getShaderManager()->getShader(type);

  // set shader locations for object
  modelToWorldLoc_ = glGetUniformLocation(shader_.getProgram(), "modelToWorld");
}

void Object::initBuffers()
{
  // bind with default data
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), indices_.data(), GL_DYNAMIC_DRAW);

  // send vert info
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  // send normal info
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));

  // unbind
  glBindVertexArray(0);
}

void Object::genFaceNormals()
{
  faceNormals_.clear();

  for (int i = 0; i < indices_.size(); i += 3)
  {
    const vec3& A = vertices_[indices_[i]].position;
    const vec3& B = vertices_[indices_[i + 1]].position;
    const vec3& C = vertices_[indices_[i + 2]].position;

    vec3 center =
    {
      (A.x + B.x + C.x) / 3,
      (A.y + B.y + C.y) / 3,
      (A.z + B.z + C.z) / 3,
    };

    // find the normal of this triangle
    vec3 vectorAB = vec3(B - A);
    vec3 vectorBC = vec3(C - B);
    vec3 norm = glm::cross(vectorAB, vectorBC);
    norm = glm::normalize(norm);

    faceNormals_.push_back(norm);
  }
}

void Object::genVertexNormals()
{
  // ensure the face normals are generated
  genFaceNormals();

  // average the face normals for vertex normal
  for (int i = 0; i < faceNormals_.size(); i++)
  {
    // apply the normal that affects these vertices
    vec3 norm = faceNormals_[i];
    for (int j = 0; j < 3; j++)
    {
      vertices_[indices_[(3 * i) + j]].normal += norm;
    }
  }

  // normalize dem normals
  for (Vertex& vert : vertices_)
  {
    vert.normal = glm::normalize(vert.normal);
  }
}



