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

  // shader setup
}

void Object::loadOBJ(std::string location)
{
}

void Object::loadeCube(float radius)
{
}

void Object::loadSphere(float radius, int divisions)
{
}

void Object::loadCircle(float radius, int divisions, glm::vec3 axis)
{
}

void Object::loadPlane(float radius)
{
}

void Object::loadTexture(std::string location, int slot)
{
}

void Object::translate(glm::vec3 trans)
{
}

void Object::rotate(float degrees, float radius, glm::vec3 axis)
{
}

void Object::scale(glm::vec3 scale)
{
}

void Object::draw()
{
  // bind everything we're using
  //glUseProgram(shaderProgram_);
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
  //glUseProgram(0);
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

void ObjectManager::render()
{
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
