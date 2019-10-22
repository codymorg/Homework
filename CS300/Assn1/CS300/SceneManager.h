/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

Purpose :   Create and manage Scenes for assignments
Language:   C++ Visual Studio
Platform:   Windows 10
Project :   cody.morgan_CS300_2
Author  :   Cody Morgan  ID: 180001017
Date    :   4 OCT 2019
End Header --------------------------------------------------------*/


#ifndef Scene_MANAGER_H
#define Scene_MANAGER_H
#pragma once

enum class SceneState
{
  Invalid = -1,
  Assignment1,
  Assignment2,
  Shutdown
};

//***** Scene Manager Functionality *****//
typedef SceneManager SceneManager;
SceneManager* GetSceneManager();

#endif

