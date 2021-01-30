/*----------------------------------------------------------------------------*\
| FILE NAME: EntityFactory.cpp                                                 |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/
#include "ECS.h"
#include "Graphics_Manager.h"
#include "Sprite_Manager.h"
#include "PhysicsManager.h"
#include "TransformManager.h"
#include "InputManager.h"
#include "ColliderManager.h"
#include "StudioClass.h"
/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

EntityFactory* EntityFactory::entityFactory_ = nullptr;

/*============================================================================*\
|| -------------------------- Private FUNCTIONS ----------------------------- ||
\*============================================================================*/

static void createPlayer(Entity* ent);

/*============================================================================*\
|| -------------------------- GLOBAL FUNCTIONS ------------------------------ ||
\*============================================================================*/

/*----------------------------------------------------------------------------*\
| Function : createEntity                                                      |
|------------------------------------------------------------------------------|
| Description : Create a new Entity                                            |
| Inputs      : type - the type of Entity to make                              |
| Outputs     : pointer to the Entity created                                  |
\*----------------------------------------------------------------------------*/
Entity* EntityFactory::CreateEntity(std::string type)
{
  Entity* newEntity = new Entity;

  newEntity->type = type;

  if (type == "Player")
  {
    createPlayer(newEntity);
  }

  entityList_[newEntity->ID] = newEntity;

  return newEntity;
}

/*----------------------------------------------------------------------------*\
| Function : getEntity                                                         |
|------------------------------------------------------------------------------|
| Description : Gets an entity from the Entity List                            |
| Inputs      : ID - GUID of the entity to get                                 |
| Outputs     : pointer to the Entity                                          |
\*----------------------------------------------------------------------------*/
Entity* EntityFactory::GetEntity(VGUID ID)
{
  return entityList_[ID];
}

EntityFactory* EntityFactory::GetEntityFactory()
{
  if(!entityFactory_)
    entityFactory_ = new EntityFactory;

  return entityFactory_;
}


static void createPlayer(Entity * ent)
{
  ent->AddComponent(new Sprite());
  ent->AddComponent(TransformCreate());
  ent->AddComponent(PhysicsCreate());
  ent->AddComponent(InputCreate(0, M_INPUT_CONTROLLER));
  ent->AddComponent(BoxColliderCreate(XMFLOAT2(0, 0), XMFLOAT2(2, 2)));
}