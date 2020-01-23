#include "ObjectManager.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
    objectManager_->genFBO();
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
  deferredRender(camera);
  //forwardRender(camera);
}

void ObjectManager::deferredRender(Camera& camera)
{
  // bind fbo 1
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_.fbo_id);
  glClearColor(1, 1, 1, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //update camera data in shaders
  ShaderManager::getShaderManager()->updateShaders(camera);

  // draw objects to textures
  for (Object* obj : objects_)
  {
    // dont draw debug objects
    if (obj->isDebugObject == false)
    {
      ShaderType original = obj->getShader().getShaderType();
      obj->setShader(ShaderType::Deferred);
      obj->draw();
      obj->setShader(original);
    }
  }

  // bind fbo 0
  //glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_.fbo_id);
  //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set up textures and their samplers for deferred drawing
  glUseProgram(ShaderManager::getShaderManager()->getShader(ShaderType::DeferredLighting).getProgram());
  for (int i = 0; i < fbo_.textureCount; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, fbo_.textures[i]);
    glUniform1i(fbo_.texSamplerLocs[i], i);
  }
  glUseProgram(0);

  // draw each object with texture lookup
  for (Object* obj : objects_)
  {
    // dont draw debug objects
    if (obj->isDebugObject == false)
    {
      ShaderType original = obj->getShader().getShaderType();
      obj->setShader(ShaderType::DeferredLighting);
      obj->draw();
      obj->setShader(original);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void ObjectManager::forwardRender(Camera& camera)
{
  // clear fbo 1
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //update camera data in shaders
  ShaderManager::getShaderManager()->updateShaders(camera);

  // draw objects to textures
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


void ObjectManager::genFBO()
{
  // gen our FBO
  glGenFramebuffers(1, &fbo_.fbo_id);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_.fbo_id);

  // init window size for textures
  GLFWwindow* window = glfwGetCurrentContext();
  glfwGetWindowSize(window, &fbo_.width, &fbo_.height);

  // Create the gbuffer textures
  glGenTextures(fbo_.textureCount, &fbo_.textures[0]);

  // fill the textures
  for (unsigned int i = 0; i < fbo_.textureCount; i++)
  {
    glBindTexture(GL_TEXTURE_2D, fbo_.textures[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, fbo_.width, fbo_.height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, fbo_.textures[i], 0);

    // these samplers are only for the deferred lighting so they can be stored in the manager
    string texName = "texSampler" + std::to_string(i);
    int deferredLightingSP = ShaderManager::getShaderManager()->getShader(ShaderType::DeferredLighting).getProgram();
    fbo_.texSamplerLocs[i] = glGetUniformLocation(deferredLightingSP, texName.c_str());

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  // Now bind the depth attachment
  glGenRenderbuffers(1, &fbo_.rbo_id);
  glBindRenderbuffer(GL_RENDERBUFFER, fbo_.rbo_id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbo_.width, fbo_.height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_.rbo_id);
  GLenum buffers[] =
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
  };
  glDrawBuffers(fbo_.textureCount, buffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("gbuffer program failed to compile");
    assert(false);
  }

  // restore default FBO
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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



