#pragma once
#ifndef _ROBO_PCH
#define _ROBO_PCH

#include <RoboCatShared.h>

#include <SDL.h>

#include <InputManager.h>

#include <Texture.h>
#include <TextureManager.h>
#include <SpriteComponent.h>
#include <RenderManager.h>
#include <GraphicsDriver.h>
#include <WindowManager.h>

#include <RoboCatClient.h>
#include <MouseClient.h>
#include <YarnClient.h>
#include "grenadeClient.h"

#include <HUD.h>


#include <ReplicationManagerClient.h>
#include <NetworkManagerClient.h>
#include <Client.h>


Vector2 WorldToScreen(Vector3 position);

#endif

