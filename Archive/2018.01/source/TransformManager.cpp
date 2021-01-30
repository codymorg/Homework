/*----------------------------------------------------------------------------*\
| FILE NAME: TransformManager.cpp                                              |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

#include "TransformManager.h"
#include "Entity.h"

TransformManager* TransformManager::transformManager_ = nullptr;

void TransformManager::Add(Transform* trans)
{
    componentList_.push_back(trans);
}

void TransformManager::Remove(Transform* trans)
{
	for (unsigned i = 0; i < componentList_.size(); i++)
	{
		if (componentList_[i] == trans)
		{
			componentList_.erase(componentList_.begin() + i);
			return;
		}
    }

}

void TransformManager::Update(float dt)
{
  for(unsigned i = 0; i < componentList_.size(); i++)
  {
    componentList_[i]->matrix = DirectX::XMMatrixTransformation2D(componentList_[i]->translation,
                                                            1,
                                                            componentList_[i]->scale,
                                                            componentList_[i]->translation,
                                                            componentList_[i]->rotation,
                                                            componentList_[i]->translation);
  }
}

TransformManager* TransformManager::GetTransformManager()
{
  if(transformManager_)
    return transformManager_;
  else
  {
    transformManager_ = new TransformManager;
    return transformManager_;
  }
}
