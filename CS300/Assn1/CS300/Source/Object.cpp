/******************************************************************************
  Brief   : manages and contains all information about the 3D object
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#include "Object.h"

static ObjectManager* objectManager = nullptr;


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

  // init openGL
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
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
}

void Object::initBuffers()
{
  
}


/////***** Object Manager stuff *****/////

ObjectManager ObjectManager::getObjectManager()
{
  return ObjectManager();
}

void ObjectManager::update()
{
}

void ObjectManager::render()
{
}

std::vector<Object*> ObjectManager::getObjectsByName(std::string name)
{
  return std::vector<Object*>();
}
