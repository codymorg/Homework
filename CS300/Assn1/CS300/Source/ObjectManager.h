
#ifndef OBJ_MAN_H
#define OBJ_MAN_H
#pragma once

#include <string>
#include <vector>

typedef class Object Object;
typedef class Camera Camera;
typedef class Light Light;

struct UBO
{
  unsigned       id = 0;
  const unsigned lightMax = 16;
  unsigned       lightCount = 0;
  unsigned       size = 0;
};

class FBO
{
public:
  void bind();
  void unbind();
  void updateTextureInfo();
  void useDepth();
  static constexpr int textureCount = 5;

  unsigned fbo_id = 0;
  unsigned rbo_id = 0;
  unsigned textures[textureCount] = {};
  int      width = -1;
  int      height = -1;
  int      texSamplerLocs[textureCount] = {};
};

class ObjectManager
{
public:
  static ObjectManager* getObjectManager();
  ~ObjectManager();

  void    render(Camera& camera);
  void    removeAllObjects();
  bool    isValid();
  Object* addObject(std::string ID = "anon_Obj");
  Object* addLight(std::string ID = "anon_Light");

  template <class T>
  Object* addVolume(Object* model, std::string ID = "root_Volume")
  {
    T* bv= new T(model, ID);
    Object* newObj = dynamic_cast<Object*>(bv);
    objects_.push_back(bv);
    return newObj;
  };

  // GPU management
  void genUBO();
  void resetUBO();
  void updateUBO(Light* light);
  void updateUBO(Object* light);

  void genFBO();

  // getters
  const std::vector<const char*> getObjectNames();
  std::vector<Object*>           getObjectsByName(std::string name);
  Object*                        getFirstObjectByName(std::string name);
  Object*                        getAt(unsigned index);
  Object*                        getSelected();
  unsigned                       getSize();

  // data
  int selectedObject = 0; // which object the GUI is looking at
  bool debugMode = false; // false: draw forward objects with depth, true: draw forward objects without depth ie on top

  // deferred data


private:
  ObjectManager() {};
  static ObjectManager* objectManager_;

  std::vector<Object*> objects_;
  bool                 isValid_ = false;

  void deferredRender(Camera& camera);
  void forwardRender(Camera& camera, bool clear = true);

  // uniform data namely lights
  UBO ubo_;
  FBO fbo_;

};

#endif