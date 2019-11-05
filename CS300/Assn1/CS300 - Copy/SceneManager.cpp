/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Create and manage Scenes for assignments
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_2
Author  :   Cody Morgan  ID: 180001017
Date    :   4 OCT 2019
End Header --------------------------------------------------------*/

#include "SceneManager.h"
#include "ObjectManagement.h"
#include "Light.h"
#include <vector>
using std::vector;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using glm::vec3;
using glm::vec4;

#include <iostream>
using std::cout;

#include <fstream>
using std::ifstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

class Scene
{
public:

  virtual void init(); 
  virtual void update(); 
  virtual void shutdown(); 

private:
  vector<Object> objects_;
  vector<Light> lights_;
};

class SceneManager
{
public:
  SceneManager()
  {}

  void Update()
  {
    // shut everything down
    if (currentState == SceneState::Shutdown)
    {
      scenes[int(currentState)].shutdown();
      Shutdown();
    }

    // run as normal
    else if (currentState == nextState)
    {
      scenes[int(currentState)].update();
    }

    // change scene
    else if (currentState != nextState)
    {
      scenes[int(currentState)].shutdown();
      scenes[int(nextState)].init();
      currentState = nextState;
    }
  }

  void Shutdown()
  {
    delete sceneManager;
    sceneManager = nullptr;
  }
  


private:
  SceneState currentState = SceneState::Invalid;
  SceneState nextState = SceneState::Invalid;

  vector<Scene> scenes = 
  {
    Assignment1,

  };
};
static SceneManager* sceneManager = nullptr;


SceneManager* GetSceneManager()
{
  if (!sceneManager)
  {
    sceneManager = new SceneManager();
  }
  return sceneManager;
}


//***** Assignment 1 *****//
class Assinment Assignment1
{
  
}
