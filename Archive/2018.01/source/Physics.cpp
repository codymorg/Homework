/*----------------------------------------------------------------------------*\
| FILE NAME: Physics.cpp                                                       |
|==============================================================================|
| CONTRIBUTER(S): Ryan Dugie                                                   |
| Copyright © 2018 DigiPen (USA) Corporation.                                  |
\*----------------------------------------------------------------------------*/

#include "PhysicsManager.h"

Physics::Physics(float xSpeed, float ySpeed, float zSpeed, float xAcceleration, float yAcceleration, float zAcceleration)
{

  velocity = {xSpeed, ySpeed, 0};

  acceleration = {xAcceleration, yAcceleration, 0};

  PhysicsManager* m = PhysicsManager::GetPhysicsManager();
  m->Add(this);
}

void Physics::SetVelocity(float x, float y, float z)
{
  velocity = {x, y, z};
}

void Physics::SetAcceleration(float x, float y, float z)
{
  acceleration = {x, y, z};
}

float Physics::GetVelocityX()
{
  return DirectX::XMVectorGetX(velocity);
}

float Physics::GetVelocityY()
{
  return DirectX::XMVectorGetY(velocity);
}

float Physics::GetVelocityZ()
{
  return DirectX::XMVectorGetZ(velocity);
}

float Physics::GetAccelerationX()
{
  return DirectX::XMVectorGetX(acceleration);
}

float Physics::GetAccelerationY()
{
  return DirectX::XMVectorGetY(acceleration);
}

float Physics::GetAccelerationZ()
{
  return DirectX::XMVectorGetZ(acceleration);
}

Physics* PhysicsCreate(float xSpeed        ,
                       float ySpeed        ,
                       float zSpeed        ,
                       float xAcceleration ,
                       float yAcceleration ,
                       float zAcceleration )
{
  return new Physics(xSpeed, ySpeed, zSpeed, xAcceleration, yAcceleration, zAcceleration);
}
