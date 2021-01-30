#include "Collider.h"
#include "ColliderManager.h"
#include "Collider_collision.h"


using namespace DirectX;

/*Functions for Collider class*/


Collider::Collider()
{
  ColliderManager* m = ColliderManager::GetColliderManger();
  m->Add(this);
}


bool Collider::CheckCollision(Collider* c1, Collider* c2) {
	return B_B_ColliderCheck_Easy(c1, c2);
}

Collider_Type Collider::ColliderGetType() {
	return m_colliderType;
}
XMFLOAT2 Collider::ColliderGetOffset() {
	return m_offset;
}
XMFLOAT2 Collider::CollidergetTransform() {
	/*Functions to get the transform from the current object */
	return XMFLOAT2(0, 0);
}

XMFLOAT2 Collider::CollidergetOldTransform() {
	return XMFLOAT2(0, 0);
}
