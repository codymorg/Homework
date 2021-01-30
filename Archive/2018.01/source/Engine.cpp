/******************************************************************************/
/*!
\file   Engine.cpp
\author Seth Kohler
\par    email: seth.kohler@digipen.edu
\par    DigiPen login: seth.kohler

*/
/******************************************************************************/


#include "Engine.h"
#include "Log.h"
#include "Entity.h"
#include "Graphics_Manager.h"
#include "Sprite_Manager.h"
#include "Transform.h"
#include "PhysicsManager.h"
#include "TransformManager.h"
#include "InputManager.h"
#include "ColliderManager.h"
#include "StudioClass.h"
#include "LevelOne.h"

Sprite_Manager*   gSpriteManager;
TransformManager* gTransformManager;
ColliderManager*  gColliderManager;
InputManager*     gInputManager;
StudioClass*      gStudioClass;
PhysicsManager*   gPhysicsManager;

// TODO (SETH) determine dependancy startup / shutdown
Engine::Engine()
{
  gSpriteManager    = Sprite_Manager::GetSpriteManager();
  gTransformManager = TransformManager::GetTransformManager();
  gColliderManager  = ColliderManager::GetColliderManger();
  gInputManager     = InputManager::GetInputManager();
  gStudioClass      = StudioClass::GetStudioClass();
  gPhysicsManager   = PhysicsManager::GetPhysicsManager();
}

Engine::~Engine()
{
}

void Engine::Initialize()
{
  InitDevice();  // initialize the gpuzzz
  gSpriteManager->Initialize();
  gTransformManager->Initialize();
  gColliderManager->Initialize();
  gInputManager->Initialize();
  gStudioClass->Initialize();
  gPhysicsManager->Initialize();

  LevelOne::GetLevelOne()->Initialize();  // TODO (RYAN) TEMPORARY
}

void Engine::Update(float dt)
{
  gSpriteManager->Update(dt);
  gTransformManager->Update(dt);
  gColliderManager->Update(dt);
  gInputManager->Update(dt);
  gStudioClass->Update(dt);
  gPhysicsManager->Update(dt);

  Render();  // render the current swapchain SHOULD ALWAYS BE SINGULAR AND CALLED LAST
}

void Engine::Shutdown()
{
  gStudioClass->Shutdown();
  gPhysicsManager->Shutdown();
  gInputManager->Shutdown();
  gColliderManager->Shutdown();
  gTransformManager->Shutdown();
  gSpriteManager->Shutdown();

  CleanupDevice();  // clean up gpu
}
