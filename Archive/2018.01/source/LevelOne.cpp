/*----------------------------------------------------------------------------*\
| FILE NAME: LevelOne.cpp                                                      |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

#include "LevelOne.h"
#include "Entity.h"
#include "Graphics_Manager.h"
#include "Sprite_Manager.h"
#include "Transform.h"
#include "PhysicsManager.h"
#include "TransformManager.h"
#include "InputManager.h"
#include "ColliderManager.h"
#include "StudioClass.h"
#include "EntityFactory.h"
#include <string>

LevelOne* LevelOne::levelOne_ = nullptr;

LevelOne::LevelOne()
{

}

LevelOne::~LevelOne()
{

}

void LevelOne::Initialize()
{
  InitTexture();
	EntityFactory::GetEntityFactory()->CreateEntity("Player");
}

void LevelOne::Update(float dt)
{

}

void LevelOne::Shutdown()
{

}

LevelOne* LevelOne::GetLevelOne()
{

  if(!levelOne_)
    levelOne_ = new LevelOne;

  return levelOne_;
}
