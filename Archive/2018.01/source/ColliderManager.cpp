#include "ColliderManager.h"
#include "Entity.h"
#include "Transform.h"
#include "Physics.h"


ColliderManager* ColliderManager::colliderManager = nullptr;

ColliderManager* ColliderManager::GetColliderManger()
{
  if(colliderManager)
    return colliderManager;
  else
  {
    colliderManager = new ColliderManager();
    return colliderManager;
  }
}

void ColliderManager::Add(Collider* ent) {
  componentList_.push_back(ent);
}

void ColliderManager::Remove(VGUID ID){

}

void ColliderManager::Update(float dt) {
	if (componentList_.size() < 2)
		return;
	std::vector<Collider*>::iterator it;
	std::vector<Collider*>::iterator that;
	for (it = componentList_.begin(); it != componentList_.end() - 1; ++it)
	{
		for (that = (it + 1); that != componentList_.end(); ++that )
		{
			if (Collider::CheckCollision(*it, *that))
			{
				if ((*it)->parent->GetComponent<Physics>()) {
					(*it)->parent->GetComponent<Transform>()->translation = (*it)->parent->GetComponent<Physics>()->oldTranslation;
					(*it)->parent->GetComponent<Physics>()->SetVelocity(0, 0, 0);
				}
				if ((*that)->parent->GetComponent<Physics>()) {
					(*that)->parent->GetComponent<Transform>()->translation = (*that)->parent->GetComponent<Physics>()->oldTranslation;
					(*that)->parent->GetComponent<Physics>()->SetVelocity(0, 0, 0);
				}
			}
		}
	}
}