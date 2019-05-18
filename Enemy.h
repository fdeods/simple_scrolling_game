#pragma once

#include "Object.h"

class Enemy;
using EnemyPtr = std::shared_ptr<Enemy>;
using EnemyPtrVec = std::vector<EnemyPtr>;

class Enemy : public Object
{
public:
  explicit Enemy(SDL_Surface* screen, Sint16 startX, Sint16 startY);

  static EnemyPtr create(SDL_Surface* screen, Sint16 startX, Sint16 startY)
  {
    return std::make_shared<Enemy>(screen, startX, startY);
  }

  void move(CollissionInfo const& collision);
  bool collision(SDL_Rect const& other);
private:
  int enemyvx, enemyvy;
  bool inAir = false;

  unsigned int frameCounter = 0;
  bool rightDirection = true;
};