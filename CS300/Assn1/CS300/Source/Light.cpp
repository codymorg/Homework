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
  isDebugObject = true;
}

void Light::draw()
{
  lightData.position = glm::vec4(this->getWorldPosition(),1);

  ObjectManager::getObjectManager()->updateUBO(this);

  // only draw this object if it is a debug object
  if (isDebugObject)
  {
    Object::draw();
  }

}
