#pragma once
#ifndef GraphicsManager_
#define GraphicsManager_
#include "Sprite.h"

class Sprite;

struct ConstantBuffer
{
  XMMATRIX mWorld;
  XMMATRIX mView;
  XMMATRIX mProjection;
};

HRESULT InitDevice();
void CleanupDevice();
void Render(Sprite* sp);
void registerSprite(Sprite* sp);

ID3D11Device* GetDevicePointer();
ID3D11DeviceContext* GetDeviceContext();
void UpdateConstantBuffer(ConstantBuffer* localConstantBuffer);

#endif