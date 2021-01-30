#include <algorithm>

#include "Sprite.h"
#include "Sprite_Manager.h"

Sprite_Manager* Sprite_Manager::spriteManager = nullptr;

Sprite_Manager* Sprite_Manager::GetSpriteManager()
{
  if(spriteManager)
    return spriteManager;
  else
  {
    spriteManager = new Sprite_Manager;
    return spriteManager;
  }
}

void Sprite_Manager::Add(Sprite* sprite)
{
  registeredSprites.push_back(sprite);
}

void Sprite_Manager::Initialize()
{
}

void Sprite_Manager::Update(float dt)
{
  UNREFERENCED_PARAMETER(dt);
  // keep the list sorted based on its tag
  registeredSprites.sort(GreaterThan);

  for(std::list<Sprite *>::iterator it = registeredSprites.begin(); it != registeredSprites.end(); it++)
  {
    // prune all dead sprites
    if((*it)->isDestroyed())
    {
      registeredSprites.erase(it);
    }
    // draw all alive sprites
    else
    {
      (*it)->Draw();
    }
  }
}

void Sprite_Manager::Shutdown()
{
}