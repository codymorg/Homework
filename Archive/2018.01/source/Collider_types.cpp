#include "Collider_types.h"

using namespace DirectX;

BoxCollider::BoxCollider(XMFLOAT2 offset, XMFLOAT2 size) {
	m_colliderType = BBOX_COLLIDER;
	m_offset = offset;
	m_size = size;
}

CircleCollider::CircleCollider(DirectX::XMFLOAT2 offset, float radius) {
	m_colliderType = CIRC_COLLIDER;
	m_offset = offset;
	m_radius = radius;
}

LineCollider::LineCollider(DirectX::XMFLOAT2 offset, DirectX::XMFLOAT2 posA, DirectX::XMFLOAT2 posB)
{
	m_colliderType = LINE_COLLIDER;
	m_offset = offset;
	m_pointA = posA;
	m_pointB = posB;
}

float CircleCollider::CircleColliderGetRadius() {
	return m_radius;
}

XMFLOAT2 BoxCollider::BoxColliderGetSize() {
	return m_size;
}



Collider* BoxColliderCreate(XMFLOAT2 offset, XMFLOAT2 sizes)
{
	return new BoxCollider(offset,sizes);
}
Collider* LineColliderCreate(XMFLOAT2 offset, XMFLOAT2 posA, XMFLOAT2 posB)
{
	return new LineCollider(offset,  posA, posB);
}

Collider* CircleColliderCreate(XMFLOAT2 offset, float radius) {
	return new CircleCollider(offset, radius);
}