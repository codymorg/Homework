/******************************************************************************
  Project : Create and manage different scenes
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <string>



class Scene
{
public:
  Scene(std::string name);

  void init();
  void update(float dt);
  void shutdown();

  // Getters

  // Setters

  // Data
  bool pause = false;

private:
  std::string name_;


};

#endif