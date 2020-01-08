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

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

ObjectManager* ObjectManager::objectManager_ = nullptr;


/////***** Vertex stuff *****/////

Vertex::Vertex(vec3 pos) : position(pos), normal(pos)
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
    translate(-centroid);
    scale(vec3(1 / modelScale));

    // load data into vertex struct
    for (size_t i = 0; i < verts.size(); i += 3)
    {
      //generate normals if not provided
      vec3 vertData(verts[i], verts[i + 1], verts[i + 2]);
      vec3 norm(modelToWorld_* vec4(vertData,1));
      vertices_.push_back(Vertex(vertData, norm));
    }

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

void Object::setShader(std::string vs, std::string fs, ShaderType type)
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


/////***** Object Manager stuff *****/////

ObjectManager* ObjectManager::getObjectManager()
{
  if(!objectManager_)
    objectManager_ = new ObjectManager;

  return objectManager_;
}

ObjectManager::~ObjectManager()
{ 
  delete objectManager_;
  objectManager_ = nullptr;
}

void ObjectManager::render(Camera& camera)
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update shaders
  ShaderManager::getShaderManager()->updateShaders(camera);

  // draw each object
  for(Object obj : objects_)
  {
    obj.draw();
  }
}

Object* ObjectManager::addObject(std::string ID)
{
  objects_.push_back(Object(ID));

  return &objects_.back();
}

std::vector<Object*> ObjectManager::getObjectsByName(std::string name)
{
  vector<Object*> namedObjects;

  // retern all objects with this name
  for (int i = 0; i < objects_.size(); i++)
  {
    if(objects_[i].name == name)
      namedObjects.push_back(&(objects_[i]));
  }

  return namedObjects;
}

Object* ObjectManager::getFirstObjectByName(std::string name)
{
  // retern first object with this name
  for (Object& obj : objects_)
  {
    if (obj.name == name)
      return &obj;
  }

  return nullptr;
}

Object* ObjectManager::getAt(unsigned index)
{
  return &objects_[index];
}

Object* ObjectManager::getSelected()
{
  return &objects_[selectedObject];
}

unsigned ObjectManager::getSize()
{
  return objects_.size();
}

