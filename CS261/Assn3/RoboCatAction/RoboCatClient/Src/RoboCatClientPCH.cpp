#include <RoboCatClientPCH.h>

Vector2 WorldToScreen(Vector3 position)
{
  return Vector2((position.mX + HALF_WORLD_WIDTH) / (2 * HALF_WORLD_WIDTH) * SCREEN_WIDTH,
    (position.mY + HALF_WORLD_HEIGHT) / (2 * HALF_WORLD_HEIGHT) * SCREEN_HEIGHT);
}
