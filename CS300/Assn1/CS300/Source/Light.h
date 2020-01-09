/******************************************************************************
  Project : Create and manage different lights
  Name    : Cody Morgan
  Date    : 16 DEC 2019
******************************************************************************/

#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

class Light : public Object
{
public:
  void draw();

  // data
  bool showLightSource = true;

private:

};

class LightManager
{
public:
  static LightManager* getLightManager();
  ~LightManager();

  const unsigned lightMax = 16;


private:
  LightManager() {};
  static LightManager* lightManager_;



};

#endif

