#pragma once

#include "Object.h"

#include <vector>

class Bullet;
using BulletPtr = std::shared_ptr<Bullet>;
using BulletPtrVec = std::vector<BulletPtr>;

class Bullet : public Object
{
public:
  explicit Bullet(SDL_Surface* screen, Sint16 startX, Sint16 startY, bool rightDirection);

  static BulletPtr create(SDL_Surface* screen, Sint16 startX, Sint16 startY, bool rightDirection)
  {
    return std::make_shared<Bullet>(screen, startX, startY, rightDirection);
  }

  void move();
private:
  const bool rightDirection = true;
};