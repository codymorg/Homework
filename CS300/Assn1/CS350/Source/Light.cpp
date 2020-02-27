/******************************************************************************
  Project : Create and manage different lights
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#include "Light.h"
#include "ObjectManager.h"

Light::Light(std::string name) : Object(name)
{
  loadSphere(1, 50);
  this->setShader(ShaderType::Normal);
}

void Light::draw()
{
  this->Light::update();
  Object::draw();
}

void Light::update()
{
  lightData.position = glm::vec4(this->getWorldPosition(), 1);

  ObjectManager::getObjectManager()->updateUBO(this);

}
