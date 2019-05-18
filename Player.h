#pragma once

#include "Object.h"
#include "Bullet.h"

class Player;
using PlayerPtr = std::shared_ptr<Player>;

class Player : public Object
{
public:
  explicit Player(SDL_Surface* screen, Sint16 startX, Sint16 startY);

  void move(DirectionInfo const& movement, CollissionInfo const& collision);
  BulletPtr fire();
private:
  int playervx, playervy;
  Sint16 playerPosition;
  bool inAir = false;

  unsigned int frameCounter = 0;
  bool rightDirection = true;
};