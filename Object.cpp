#include "Object.h"

#include <tuple>

Object::Object(SDL_Surface* scr, const char* filePath, Sint16 x, Sint16 y, Uint16 w, Uint16 h)
: screen(scr),
    image(SurfacePtr(SDL_LoadBMP(filePath), [=](SDL_Surface* surf){SDL_FreeSurface(surf);})),
    position{x,y,w,h},
    currentFrame{0,0,w,h},
    startX(x),
    startY(y)
{  
  if (image)
    SDL_SetColorKey(image.get(), SDL_SRCCOLORKEY, SDL_MapRGB(image->format, 0x00, 0xff, 0xff));
}

void Object::render(Sint16 cameraOffset)
{
  if (image) {
    SDL_Rect tmpCameraPosition{static_cast<Sint16>(position.x-cameraOffset), position.y, position.w, position.h};
    SDL_BlitSurface(image.get(), &currentFrame, screen, &tmpCameraPosition);
  }
}

SDL_Rect const& Object::getPosition() const
{
    return position;
}
