/*----------------------------------------------------------------------------*\
| FILE NAME: PhysicsManager.cpp                                                |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

#include "PhysicsManager.h"
#include "Entity.h"
#include "Transform.h"

PhysicsManager* PhysicsManager::physicsManager_ = nullptr;

void PhysicsManager::Add(Physics* phy)
{
  componentList_.push_back(phy);
}

void PhysicsManager::Remove(Physics* phy)
{
  for(unsigned i = 0; i < componentList_.size(); i++)
  {
    if(componentList_[i] == phy)
    {
      componentList_.erase(componentList_.begin() + i);
      return;
    }
  }
}

void PhysicsManager::Update(float dt)
{

  for(unsigned i = 0; i < componentList_.size(); i++)
  {
    Transform* transform = componentList_[i]->parent->GetComponent<Transform>();

	componentList_[i]->oldTranslation = transform->translation;

	componentList_[i]->velocity = DirectX::XMVectorAdd(componentList_[i]->velocity, componentList_[i]-> acceleration);

	transform->translation = DirectX::XMVectorAdd(componentList_[i]->velocity, transform->translation);

  }
}

PhysicsManager* PhysicsManager::GetPhysicsManager()
{
  if(physicsManager_)
    return physicsManager_;
  else
  {
    physicsManager_ = new PhysicsManager;
    return physicsManager_;
  }
}
