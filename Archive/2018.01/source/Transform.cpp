/*----------------------------------------------------------------------------*\
| FILE NAME: Transform.h                                                       |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/


#include "TransformManager.h"

Transform::Transform(float x, float y, float z, float xScale, float yScale, float angle)
{
  scale = {xScale, yScale, 1};
  translation = {x, y, z};
  rotation = angle;

  TransformManager* m = TransformManager::GetTransformManager();
  m->Add(this);
}

void Transform::SetTranslation(float x, float y, float z)
{
  translation = {x, y, z};
}

void Transform::Setscale(float x, float y)
{
  scale = {x, y, 1};
}

float Transform::GetTranslationX()
{
  return DirectX::XMVectorGetX(translation);
}

float Transform::GetTranslationY()
{
  return DirectX::XMVectorGetY(translation);
}

float Transform::GetTranslationZ()
{
  return DirectX::XMVectorGetZ(translation);
}

float Transform::GetScaleX()
{
  return DirectX::XMVectorGetX(scale);
}

float Transform::GetScaleY()
{
  return DirectX::XMVectorGetY(scale);
}

Transform* TransformCreate(float x, float y, float z, float xScale, float yScale, float angle)
{
  return new Transform(x, y, z, xScale, yScale, angle);
}
