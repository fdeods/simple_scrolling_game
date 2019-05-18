#pragma once

#include "Object.h"

class Ball;
using BallPtr = std::shared_ptr<Ball>;

class Ball : public Object
{
public:
    explicit Ball(SDL_Surface* screen, const char* filePath, Sint16 startX, Sint16 startY);

    void move();
    CollisionResult collision(std::pair<SDL_Rect const&, SDL_Rect const&> players);
private:
    unsigned int ballvx, ballvy;
};