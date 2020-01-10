
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
  unsigned       id;
  const unsigned lightMax = 16;
  unsigned       lightCount = 0;
  unsigned       size = sizeof(Light::lightData) * lightMax + sizeof(Object::MaterialData);
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
  void updateLightUBO(Light* light);
  void updateMaterialUBO(Object* object);


  // getters
  std::vector<Object*> getObjectsByName(std::string name);
  Object*              getFirstObjectByName(std::string name);
  Object*              getAt(unsigned index);
  Object*              getSelected();
  unsigned             getSize();

  // data
  int selectedObject = 0; // which object the GUI is looking at

  // deferred data


private:
  ObjectManager() {};
  static ObjectManager* objectManager_;

  std::vector<Object*> objects_;
  bool                 isValid_ = false;

  // uniform data namely lights
  UBO ubo_;
};

#endif