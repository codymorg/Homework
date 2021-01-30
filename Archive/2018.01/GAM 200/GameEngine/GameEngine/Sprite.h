#pragma once
#ifndef Sprite_h
#define Sprite_h

#include "Mesh.h"
#include <string>
#include "Graphics_Manager.h"

struct ConstantBuffer;
class Mesh;

//use this to group sprites for rendering
enum spriteTag
{
  spriteTag_background,
  spriteTag_foreground, //everything is foreground by default
  spriteTag_UI,
};

class Sprite
{
public:
  Sprite(); //makes a default mesh (unit sqare wth no texture)
  ~Sprite();

  void draw();

  Mesh* mesh_;
  XMMATRIX transform_;
private:
  ConstantBuffer* thisCB_;
  std::string textureLocation_; 
  spriteTag tag = spriteTag_foreground;

  //TODO: pointer to where the texture is in memory
};

#endif