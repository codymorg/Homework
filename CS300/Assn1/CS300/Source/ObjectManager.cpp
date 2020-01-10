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
    objectManager_ = new ObjectManager;

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

  // draw each object
  for (Object* obj : objects_)
  {
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
    Light* light = new Light(ID);
    Object* lightObj = dynamic_cast<Object*>(light);
    objects_.push_back(lightObj);

    selectedObject = objects_.size() - 1;
    isValid_ = true;
    light->ID = ubo_.lightCount;
    ubo_.lightCount++;

    return objects_.back();
  }
}

// generate a UBO for light and material data
// [ [light data 0] [light data...] [material data] ]
void ObjectManager::genUBO()
{
  glGenBuffers(1, &ubo_.id);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_.id);
  glBufferData(GL_UNIFORM_BUFFER, ubo_.size, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_.id);

  // zeroize the buffer space
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY); 
  memset(buffer, 0, ubo_.size);
  glUnmapNamedBuffer(ubo_.id);
}

// update during render pass 1 so that all light data is fresh for render pass 2
void ObjectManager::updateLightUBO(Light* light)
{
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY); // TODO move this outside light update 

  // fill this light member
  Light::LightData* lightData = static_cast<Light::LightData*>(buffer) + light->ID;
  memcpy(static_cast<void*>(lightData), &light->lightData, sizeof(Light::LightData));

  glUnmapNamedBuffer(ubo_.id);
}

// update during render pass 2 as objects are drawn and again during render pass 3
void ObjectManager::updateMaterialUBO(Object* object)
{
  void* buffer = glMapNamedBuffer(ubo_.id, GL_WRITE_ONLY);

  // fill this light member
  Light::LightData* materialData = static_cast<Light::LightData*>(buffer) + ubo_.lightMax + 1;
  memcpy(static_cast<void*>(materialData), &object->material, sizeof(Object::MaterialData));

  glUnmapNamedBuffer(ubo_.id);
}

bool ObjectManager::isValid()
{
  return isValid_;
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