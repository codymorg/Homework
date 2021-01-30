
#include "Collider_collision.h"
#include "Transform.h"
#include "Entity.h"

using namespace DirectX;

bool B_L_ColliderCheck(Collider* A, Collider* B) {
	return false;
}
bool L_B_ColliderCheck(Collider *A, Collider* B) {
	return B_L_ColliderCheck(B, A);
}

bool B_C_ColliderCheck(Collider* A, Collider* B) {
	return false;
}
bool C_B_ColliderCheck(Collider* A, Collider* B) {
	return B_C_ColliderCheck(A, B);
}

bool L_C_ColliderCheck(Collider* A, Collider* B) {
	/*bool result = false;

	LineCollider* colliderA = (LineCollider*)A;
	CircleCollider* colliderB = (CircleCollider*)B;

	if (A != NULL && B != NULL)
	{
		//ColliderLinePtr line = (ColliderLinePtr)collider;

		HXMVECTOR circle_pos = XMLoadFloat3(&B->CollidergetTransform());
		//const Vector2D* original_pos = TransformGetTranslation(GameObjectGetTransform(other->parent));
		const float radius = colliderB->CircleColliderGetRadius();
		//const float radius = ColliderCircleGetRadius(other);
		const float RR = radius * radius;
		//const float RR = radius * radius;

		for (unsigned int i = 0; i <1; ++i)
		{
			//const ColliderLineSegment* line_segment = &line->lineSegments[i];
			const HXMVECTOR p0 = XMLoadFloat3()
				//const Vector2D* p0 = line_segment->point + 0;
				const Vector2D* p1 = line_segment->point + 1;

			Vector2D circleDist;
			Vector2DSub(&circleDist, p1, p0);

			Vector2D line2point;
			Vector2DSub(&line2point, p0, original_pos);

			const float DD = Vector2DDotProduct(&circleDist, &circleDist);
			const float PD = Vector2DDotProduct(&line2point, &circleDist) * 2.0f;
			const float PP = Vector2DDotProduct(&line2point, &line2point) - RR;

			const float PDPD = (PD * PD) - (4.0f * DD * PP);

			if (PDPD >= 0.0f)
			{
				const float inverted2a = 1.0f / (2.0f * DD);
				const float sqrt_d = sqrtf(PDPD);
				const float check1 = (-PD - sqrt_d) * inverted2a;


				if (check1 >= 0.0f && check1 <= 1.0f)
				{
					result = true;
				}

				else
				{
					const float check2 = (-PD + sqrt_d) * inverted2a;

					if (check2 >= 0.0f && check2 <= 1.0f)
					{
						result = true;
					}
				}


				if (result == true)
				{
					Vector2DSet(&circleDist, -circleDist.y, -circleDist.x);
					Vector2DNormalize(&circleDist, &circleDist);

					const Vector2D* velocity = PhysicsGetVelocity(GameObjectGetPhysics(other->parent));
					const float dot = Vector2DDotProduct(velocity, &circleDist);

					Vector2D newVelocity;
					Vector2DScale(&circleDist, &circleDist, 2.0f * dot);
					Vector2DSub(&newVelocity, velocity, &circleDist);

					const float angle = atan2f(newVelocity.y, newVelocity.x);

					TransformSetRotation(GameObjectGetTransform(other->parent), angle);
					PhysicsSetVelocity(GameObjectGetPhysics(other->parent), &newVelocity);
				}
			}
		}
	}

	return result;*/
	return false;
}

bool C_L_ColliderCheck(Collider* A, Collider* B) {
	return L_C_ColliderCheck(B, A);
}
bool L_L_ColliderCheck(Collider* A, Collider* B) {
	return false;
}
bool C_C_ColliderCheck(Collider* A, Collider* B) {
	/*XMVECTOR center1 = XMVectorAdd(XMLoadFloat3(&A->ColliderGetOffset()), XMLoadFloat3(&A->CollidergetTransform()));
	XMVECTOR center2 = XMVectorAdd(XMLoadFloat3(&B->ColliderGetOffset()), XMLoadFloat3(&B->CollidergetTransform()));
	XMVECTOR diff = XMVectorSubtract(center1, center2);
	float distance;
	XMStoreFloat(&distance, XMVector3Length(diff));
	float rpr = ((CircleCollider*)A)->CircleColliderGetRadius() + ((CircleCollider*)B)->CircleColliderGetRadius();
	return distance < rpr;*/
	return false;
}
bool B_B_ColliderCheck(Collider* A, Collider* B) {
	/*XMVECTOR center1 = XMVectorAdd(XMLoadFloat3(&A->ColliderGetOffset()), XMLoadFloat3(&A->CollidergetTransform()));
	XMVECTOR center2 = XMVectorAdd(XMLoadFloat3(&B->ColliderGetOffset()), XMLoadFloat3(&B->CollidergetTransform()));
	//	XMVECTOR size1    = XMLoadFloat(A->COll)

	XMVECTOR c1_lt;
	XMVECTOR c1_lb;
	XMVECTOR c1_rt;
	XMVECTOR c1_rb;
	for (int line_A = 0; line_A < 4; line_A++)
	{
		XMVECTOR line_A_p1 = XMLoadFloat3(&A->ColliderGetOffset());
		for (int line_B = 0; line_B < 4; line_B++)
		{

		}
	}
	return false;*/
	return false;
}


bool B_B_ColliderCheck_Easy(Collider* A, Collider* B) {
	XMVECTOR center1 = XMVectorAdd(XMLoadFloat2(&A->ColliderGetOffset()),A->parent->GetComponent<Transform>()->translation);
	XMVECTOR center2 = XMVectorAdd(XMLoadFloat2(&B->ColliderGetOffset()), B->parent->GetComponent<Transform>()->translation);
	XMVECTOR size1 = XMLoadFloat2(&((BoxCollider*)A)->BoxColliderGetSize());
	XMVECTOR size2 = XMLoadFloat2(&((BoxCollider*)B)->BoxColliderGetSize());
	
	if (abs(XMVectorGetX(center1) - XMVectorGetX(center2)) > (XMVectorGetX(size1) + XMVectorGetX(size2))) return false;
	if (abs(XMVectorGetY(center1) - XMVectorGetY(center2)) > (XMVectorGetY(size1) + XMVectorGetY(size2))) return false;

	return true;
}



bool(*m_CollisionTable[COLLIDER_TYPES][COLLIDER_TYPES])(Collider*, Collider*) = {
	/*L_L_ColliderCheck,L_B_ColliderCheck,L_C_ColliderCheck,
	B_L_ColliderCheck,B_B_ColliderCheck_Easy,B_C_ColliderCheck,
	C_L_ColliderCheck,C_B_ColliderCheck,C_C_ColliderCheck*/
};