#include "Enemy.h"

Enemy::Enemy(SDL_Surface* screen, Sint16 x, Sint16 y)
  : Object(screen, ENEMYFILE, x, y, TILESIZE, TILESIZE),
    enemyvx(ENEMYVX),
    enemyvy(ENEMYVY),
    inAir(false)
{}

void Enemy::move(CollissionInfo const& collision)
{
  if (rightDirection && collision.right)
    rightDirection = false;

  if (!rightDirection && collision.left)
    rightDirection = true;

  if (rightDirection && !collision.right)
    position.x += enemyvx;
  
  if (!rightDirection && !collision.left)
    position.x -= enemyvx;

  if (!collision.down)
    position.y += enemyvy;
  
  frameCounter += ENEMYFRAMESTEP;
  currentFrame.x = ((int(frameCounter/10))%ENEMYFRAMES)*TILESIZE;
}

bool Enemy::collision(SDL_Rect const& other)
{
  // check collision with game moving objects
  return epsRectCollision(this->getPosition(), other);
}
