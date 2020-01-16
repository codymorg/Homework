/******************************************************************************
  Brief   : manages and contains all information about the 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

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
#include <iostream>
  using std::cout;

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>



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

  // init openGL
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  initBuffers();
}

void Object::loadOBJ(string fileLocation)
{
  ifstream file(fileLocation);

  // load vert data and normal data
  if (file)
  {
    vector<float>verts; // vertex numbers

    // bounding box
    vec3 maxPos = vec3(INT_MIN, INT_MIN, INT_MIN);
    vec3 minPos = vec3(INT_MAX, INT_MAX, INT_MAX);

    // since the file is good - clear the old vertex data if any
    vertices_.clear();
    indices_.clear();
    modelToWorld_ = glm::mat4();

    string line;
    while (getline(file, line))
    {
      if (line.substr(0, 2) == "v " || line.substr(0, 2) == "v\t")
      {
        std::istringstream objData(line.substr(2));
        float vertexData;
        for (size_t i = 0; i < 3; i++)
        {
          objData >> vertexData;
          verts.push_back(vertexData);
          minPos[i] = min(minPos[i], vertexData);
          maxPos[i] = max(maxPos[i], vertexData);
        }
      }
      else if (line.substr(0, 2) == "f ")
      {
        std::istringstream objData(line.substr(2));
        std::string token;
        int faceCount = 0;
        vector<unsigned> fanIndices;

        // if you have '//' delimiters you monster
        if (line.find("//") != string::npos)
        {
          while (getline(objData, token, '/'))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;

            // add this face to the fan list if you have more than 3 faces
            fanIndices.push_back(index - 1);
            indices_.push_back(index - 1);

            faceCount++;

            // skip uv stuff
            objData.get();
            char peek = '\0';
            while (peek != ' ' && peek != -1)
            {
              peek = objData.peek();
              objData.get();
            }
          }
        }

        // if you have '\\' delimiters
        else if (line.find("\\") != string::npos && line.find("\\par") == string::npos)
        {
          while (getline(objData, token, '\\'))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;

            fanIndices.push_back(index - 1);
            faceCount++;

            char peek = '\0';
            while (peek != ' ' && peek != -1)
            {
              peek = objData.peek();
              objData.get();
            }
            indices_.push_back(index - 1);
          }
        }

        // if you have ' ' delimitation like a normal person
        else
        {
          while (getline(objData, token, ' '))
          {
            int index;
            std::istringstream substring(token);
            substring >> index;
            indices_.push_back(index - 1);
            fanIndices.push_back(index - 1);

            faceCount++;

          }
        }

        if (faceCount > 3)
        {
          // shear off those fan faces
          while (faceCount > 3)
          {
            indices_.pop_back();
            faceCount--;
          }

          // convert fan to gl tris
          unsigned triA = fanIndices[0];
          for (int i = 0; i < fanIndices.size() - 3; i++)
          {
            unsigned triB = fanIndices[2 + i];
            unsigned triC = fanIndices[3 + i];
            indices_.push_back(triA);
            indices_.push_back(triB);
            indices_.push_back(triC);
          }
        }

      }
    }

    // scale down to unit size and put at the origin
    vec3 centroid = (maxPos + minPos) / 2.0f;
    float modelScale = ((maxPos - minPos).x + (maxPos - minPos).y + (maxPos - minPos).z) / 3;
    scale(vec3(2 / modelScale));
    translate(-centroid);

    // load data into vertex struct
    for (size_t i = 0; i < verts.size(); i += 3)
    {
      vec3 vertData(verts[i], verts[i + 1], verts[i + 2]);
      vertices_.push_back(Vertex(vertData));
    }
    genVertexNormals();

    initBuffers();
  }
  else
  {
    cout << "error opening file: " << fileLocation << "\n";
  }
}

void Object::loadeCube(float side)
{
  vertices_ =
  {
    Vertex(vec3(-side / 2, -side / 2,  side / 2)),
    Vertex(vec3(side / 2, -side / 2,  side / 2)),
    Vertex(vec3(side / 2,  side / 2,  side / 2)),
    Vertex(vec3(-side / 2,  side / 2,  side / 2)),
    Vertex(vec3(-side / 2, -side / 2, -side / 2)),
    Vertex(vec3(side / 2, -side / 2, -side / 2)),
    Vertex(vec3(side / 2,  side / 2, -side / 2)),
    Vertex(vec3(-side / 2,  side / 2, -side / 2))
  };

  indices_ =
  {
    0,1,2, // Front
    0,2,3,
    0,5,1, // Bottom
    0,4,5,
    1,5,6, // Right
    1,6,2,
    4,0,3, // Left
    4,3,7,
    4,7,6, // Back
    4,6,5,
    3,2,6, // Top
    3,6,7
  };

  initBuffers();
}

void Object::loadSphere(float diameter, int divisions)
{
  float radius = diameter / 2;
  // reset object geometry
  vertices_.clear();
  indices_.clear();

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

void Object::translate(glm::vec3 trans)
{
  modelToWorld_ = glm::translate(modelToWorld_, trans);
}

void Object::rotate(float degrees, glm::vec3 center, glm::vec3 axis)
{
  modelToWorld_ = glm::translate(modelToWorld_, center);
  modelToWorld_ = glm::rotate(modelToWorld_, glm::radians(degrees), axis);
  modelToWorld_ = glm::translate(modelToWorld_, -center);
}

void Object::scale(glm::vec3 scale)
{
  modelToWorld_ = glm::scale(modelToWorld_, scale);
}

void Object::draw()
{

  // bind everything we're using
  glUseProgram(shader_.getProgram());
  glBindVertexArray(vao_);

  if (!wiremode)
  {
    glPolygonMode(GL_FRONT, GL_FILL);
  }
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  // send data for drawing
  glUniform3fv(boundingBoxLoc_, 2, glm::value_ptr(*boundingBox_));
  glUniformMatrix4fv(modelToWorldLoc_, 1, GL_FALSE, &modelToWorld_[0][0]);

  // draw
  glDrawElements(renderMode_, indices_.size(), GL_UNSIGNED_INT, 0);

  // unbind
  glBindVertexArray(0);
  glUseProgram(0);
}

unsigned Object::getShaderProgram()
{
  return shader_.getProgram();
}

vec3 Object::getWorldPosition()
{
  return vec3(modelToWorld_[3]);
}

void Object::setShader(ShaderType type)
{
  shader_ = ShaderManager::getShaderManager()->getShader(type);

  // set shader locations for object
  modelToWorldLoc_ = glGetUniformLocation(shader_.getProgram(), "modelToWorld");
  assert(modelToWorldLoc_ != -1 && "model matrix location is invalid");
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


