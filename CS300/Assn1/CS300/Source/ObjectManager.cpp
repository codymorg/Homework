#include "ObjectManager.h"
#include <vector>
  using std::vector;
#include <string>
  using std::string;
#include "Light.h"

// singleton
ObjectManager* ObjectManager::objectManager_ = nullptr;


ObjectManager* ObjectManager::getObjectManager()
{
  if (!objectManager_)
  {
    objectManager_ = new ObjectManager;
    objectManager_->genUBO();
  }

  return objectManager_;
}

ObjectManager::~ObjectManager()
{
  for (Object* obj : objects_)
  {
    delete obj;
  }

  delete objectManager_;
  objectManager_ = nullptr;
}

void ObjectManager::render(Camera& camera)
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update shaders
  ShaderManager::getShaderManager()->updateShaders(camera);

  //Light* i = dynamic_cast<Light*>(objects_[0]);
  // draw each object
  for (Object* obj : objects_)
  {
    if (!dynamic_cast<Light*>(obj))
      updateUBO(obj);

    obj->draw();
  }
}

void ObjectManager::removeAllObjects()
{
  selectedObject = -1;
  isValid_ = false;
  objects_.clear();
}

Object* ObjectManager::addObject(std::string ID)
{
  objects_.push_back(new Object(ID));
  selectedObject = objects_.size() - 1;
  isValid_ = true;

  return objects_.back();
}

Object* ObjectManager::addLight(std::string ID)
{
  if (ubo_.lightCount >= ubo_.lightMax)
  {
    printf("You have reached the max number of lights - no more created");
    return nullptr;
  }
  else
  {
    // add a light object to list of objects 
    Light* light = new Light(ID);
    Object* lightObj = dynamic_cast<Object*>(light);
    lightObj->loadSphere(1,50);
    objects_.push_back(light);

    // update relevant light data
    selectedObject = objects_.size() - 1;
    isValid_ = true;
    ubo_.lightCount++;
    light->ID = ubo_.lightCount;

    return objects_.back();
  }
}

void ObjectManager::genUBO()
{
  // fill in information
  ubo_.size = sizeof(Light::LightData) * ubo_.lightMax + sizeof(Object::MaterialData);
  glGenBuffers(1, &ubo_.id);

  // bind this ubo
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_.id);
  glBufferData(GL_UNIFORM_BUFFER, ubo_.size, nullptr, GL_STATIC_DRAW);

  // zeroize the buffer space
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY); // does this have to be every frame?
  memset(buffer, 0, sizeof(Light::LightData) * ubo_.lightMax);

  // release buffer
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_.id);
  glUnmapNamedBuffer(ubo_.id);
}

void ObjectManager::updateUBO(Light* light)
{
  //glBindBuffer(GL_UNIFORM_BUFFER, ubo_.id);
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY);

  // fill this light member
  Light::LightData* nextMember = static_cast<Light::LightData*>(buffer) + (light->ID - 1);
  memcpy(static_cast<void*>(nextMember), &light->lightData, sizeof(Light::LightData));

  glUnmapNamedBuffer(ubo_.id);
  //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ObjectManager::updateUBO(Object* object)
{
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY);

  // get last light which coincides with the material
  Light::LightData* lastLight = static_cast<Light::LightData*>(buffer) + ubo_.lightMax;
  void* material = static_cast<void*>(lastLight);
  memcpy(material, &object->material, sizeof(Object::MaterialData));

  glUnmapNamedBuffer(ubo_.id);
  //glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool ObjectManager::isValid()
{
  return isValid_;
}

const std::vector<const char*> ObjectManager::getObjectNames()
{
  vector<const char*> names;
  for(Object* obj : objects_)
  {
    names.push_back(obj->name.c_str());
  }

  return static_cast<const vector<const char*>>(names);
}

std::vector<Object*> ObjectManager::getObjectsByName(std::string name)
{
  vector<Object*> namedObjects;

  // retern all objects with this name
  for (int i = 0; i < objects_.size(); i++)
  {
    if (objects_[i]->name == name)
      namedObjects.push_back(objects_[i]);
  }

  return namedObjects;
}

Object* ObjectManager::getFirstObjectByName(std::string name)
{
  // retern first object with this name
  for (Object* obj : objects_)
  {
    if (obj->name == name)
      return obj;
  }

  return nullptr;
}

Object* ObjectManager::getAt(unsigned index)
{
  return objects_[index];
}

Object* ObjectManager::getSelected()
{
  if (isValid_)
    return objects_[selectedObject];
  else
    return nullptr;
}

unsigned ObjectManager::getSize()
{
  return objects_.size();
}