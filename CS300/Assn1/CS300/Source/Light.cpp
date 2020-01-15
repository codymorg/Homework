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
  this->setShader(ShaderType::Passthrough);
  isDebugObject = true;
}

void Light::draw()
{
  ObjectManager::getObjectManager()->updateUBO(this);

  // only draw this object if it is a debug object
  if (isDebugObject)
  {
    Object::draw();
  }

}
