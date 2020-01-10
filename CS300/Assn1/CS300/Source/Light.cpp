/******************************************************************************
  Project : Create and manage different lights
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#include "Light.h"
#include "Object.h"

Light::Light(std::string name) : Object(name)
{
  loadSphere(1, 50);
  this->setShader(ShaderType::Passthrough);
  debugObject = true;
}
