#include "Bullet.h"

Bullet::Bullet(SDL_Surface* screen, Sint16 x, Sint16 y, bool rightDirection)
  : Object(screen, BULLETFILE, x, y, BULLETSIZE, BULLETSIZE),
    rightDirection(rightDirection)
{}

void Bullet::move()
{
  if (rightDirection)
    position.x += BULLETVX;
  else
    position.x -= BULLETVX;
}