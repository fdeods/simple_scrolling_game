#include "Ball.h"

Ball::Ball(SDL_Surface* screen, const char* filePath, Sint16 x, Sint16 y)
    : Object(screen, filePath, x, y, BALLSIZE, BALLSIZE),
      ballvx(BALLV),
      ballvy(BALLV)
{}

void Ball::move()
{
    position.x += ballvx;
    position.y += ballvy;
    
    if (position.y <= 0)
        ballvy = -ballvy;

    if (position.y+position.h>=HEIGHT)
        ballvy = -ballvy;
}

CollisionResult Ball::collision(std::pair<SDL_Rect const&, SDL_Rect const&> players)
{
    if (position.x + position.w >= WIDTH)
        return RightBorder;
    if (position.x <= 0)
        return LeftBorder;

    if (checkCollision(position, players.first))
    {
        if (position.x < players.first.x + players.first.w)
            ballvy = -ballvy;
        else
            ballvx = -ballvx;
        return Touch;
    }

    if (checkCollision(position, players.second))
    {
        if (position.x > players.second.x)
            ballvy = -ballvy;
        else
            ballvx = -ballvx;
        return Touch;
    }

    return No;
}
