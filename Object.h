#pragma once

#include "Commons.h"

class Object
{
public:
    explicit Object(SDL_Surface* screen, const char* fileName, Sint16 startX, Sint16 startY, Uint16 width, Uint16 height);
    void render(Sint16 cameraOffset);
    SDL_Rect const& getPosition() const;
protected:
    SDL_Surface* screen;
    SurfacePtr image;

    SDL_Rect position;
    unsigned int startX, startY;

    SDL_Rect currentFrame;
};