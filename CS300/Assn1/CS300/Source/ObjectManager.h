
#ifndef OBJ_MAN_H
#define OBJ_MAN_H
#pragma once

#include <string>
#include <vector>

typedef class Object;
typedef class Camera;
typedef class Light;

struct UBO
{
  unsigned       id = 0;
  const unsigned lightMax = 16;
  unsigned       lightCount = 0;
  float          size = 0;
};
struct FBO
{
  unsigned             fbo_id = 0;
  unsigned             rbo_id = 0;
  int                  width = -1;
  int                  height = -1;
  static constexpr int textureCount = 2;
  unsigned             textures[textureCount];


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

  // GPU management
  void genUBO();
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

  // deferred data


private:
  ObjectManager() {};
  static ObjectManager* objectManager_;

  std::vector<Object*> objects_;
  bool                 isValid_ = false;

  void deferredRender(Camera& camera);
  void forwardRender(Camera& camera);

  // uniform data namely lights
  UBO ubo_;
  FBO fbo_;
};

#endif