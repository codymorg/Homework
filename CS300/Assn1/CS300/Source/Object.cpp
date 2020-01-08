/******************************************************************************
  Brief   : manages and contains all information about the 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#include "Object.h"
  using std::vector;
  using glm::vec3;
  using glm::vec4;
  using glm::mat4;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

ObjectManager* ObjectManager::objectManager_ = nullptr;


/////***** Vertex stuff *****/////

Vertex::Vertex(glm::vec3 pos)
{
  position = pos;
  normal = pos;
}


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

void Object::loadOBJ(std::string location)
{
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
