#include "Player.h"

Player::Player(SDL_Surface* screen, Sint16 x, Sint16 y)
  : Object(screen, HEROFILE, x, y, TILESIZE, TILESIZE),
    playervx(PLAYERVX),
    playervy(PLAYERVY),
    playerPosition(0),
    inAir(false)
{}

void Player::move(DirectionInfo const& movement, CollissionInfo const& collision)
{
  if (movement.right && !collision.right)
  {
    position.x += playervx;
    if (rightDirection)
    {
      frameCounter += PLAYERFRAMESTEP;
    }
    else
    {
      frameCounter = 0;
      rightDirection = true;
    }
  }

  if (movement.left && !collision.left)
  {
    position.x -= playervx;
    if (!rightDirection)
    {
      frameCounter += PLAYERFRAMESTEP;
    }
    else
    {
      frameCounter = 0;
      rightDirection = false;
    }
  }

  if (!movement.right && !movement.left)
    frameCounter = 0;

  if (inAir)
  {
    if (playervy < 0)
    {
      // jumping sequence
      if (collision.up)
      {
        playervy = PLAYERVY;
      }
      else
      {
        position.y += playervy;
        ++playervy;
        if (playervy == 0) {
          playervy = PLAYERVY;
          position.y = ((position.y + PLAYERVY - 1) / PLAYERVY) * PLAYERVY;
        }
      }
    }
    else
    {
      // falling
      if (!collision.down)
      {
        position.y += playervy;
      }
      else
      {
        inAir = false;
        position.y = ((position.y + TILESIZE/2) / TILESIZE) * TILESIZE;
      }
      
    }
  }
  else
  {
    if (!collision.down)
    {
      inAir = true;
    }
    else if (movement.up && !collision.up)
    {
      inAir = true;
      playervy = -15;
    }
  }

  if (rightDirection)
    currentFrame.x = ((int(frameCounter/10))%PLAYERFRAMES)*TILESIZE;
  else
    currentFrame.x = (PLAYERFRAMES + (int(frameCounter/10))%PLAYERFRAMES)*TILESIZE;
}

BulletPtr Player::fire()
{
  if (rightDirection)
    return Bullet::create(screen, position.x+TILESIZE, position.y+TILESIZE/5*2, rightDirection);
  else
    return Bullet::create(screen, position.x, position.y+TILESIZE/5*2, rightDirection);
}
