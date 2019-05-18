#include "GameMap.h"

#include <fstream>

GameMap::GameMap(SDL_Surface* scr)
: screen(scr),
  blocksImage(SurfacePtr(SDL_LoadBMP(BLOCKSFILE), [=](SDL_Surface* surf){SDL_FreeSurface(surf);})),
  backgroundImage(SurfacePtr(SDL_LoadBMP(BACKGROUNDFILE), [=](SDL_Surface* surf){SDL_FreeSurface(surf);})),
  mapPos(0),
  backLen(backgroundImage ? backgroundImage->clip_rect.w - WIDTH : 0)
{
  loadMap();
  if (blocksImage)
    SDL_SetColorKey(blocksImage.get(), SDL_SRCCOLORKEY, SDL_MapRGB(screen->format, 0x00, 0xff, 0xff));
}

 MapPoint const& GameMap::getPlayerStartPosition()
 {
   return playerTile;
 }

MapPointVec const& GameMap::getEnemiesStartPosition()
{
  return enemyTiles;
}

void GameMap::loadMap()
{
  std::ifstream infile(MAPDESCFILE);
  if (!infile.is_open())
  {
    std::cout << "Could not open file: " << MAPDESCFILE << std::endl;
    return;
  }

  int height = 0, width = 0;
  infile >> height >> width;
  mapDescription.reserve(height);

  for (int i=0; i < height; ++i)
  {
    std::vector<int> line;
    line.reserve(width);
    int tmpNumber = 0;
    for (int j=0; j < width; ++j)
    {
      infile >> tmpNumber;
      switch (tmpNumber)
      {
        case GOALTILE:
          goal = std::make_pair(j,i);
          line.push_back(tmpNumber);
          break;
        case PLAYERTILE:
          playerTile = std::make_pair(j,i);
          line.push_back(0);
          break;
        case ENEMYTILE:
          enemyTiles.push_back(std::make_pair(j,i));
          line.push_back(0);
          break;
        default:
          line.push_back(tmpNumber);
          break;
      }
    }

    mapDescription.push_back(line);
  }

  infile.close();
}

void GameMap::updateCamera(SDL_Rect const& playerPos)
{
  if (playerPos.x > mapPos + WINDOWMAX)
    mapPos = playerPos.x - WINDOWMAX;
  else if (playerPos.x < mapPos + WINDOWMIN)
    mapPos = playerPos.x - WINDOWMIN;
}

Sint16 GameMap::cameraOffset() const
{
  return mapPos;
}

void GameMap::render()
{
  if (!backgroundImage)
    return;

  if (!blocksImage)
    return;

  SDL_Rect tmpPosRect{0, 0, TILESIZE, TILESIZE};

  SDL_Rect tmpTileRect{0, 0, TILESIZE, TILESIZE};
  SDL_Rect backFrameRect{static_cast<Sint16>(mapPos%backLen), 0, backgroundImage->clip_rect.w, backgroundImage->clip_rect.h};
  if (backFrameRect.x < 0)
    backFrameRect.x += backLen;

  SDL_BlitSurface(backgroundImage.get(), &backFrameRect, screen, &tmpPosRect);

  for (int h=0; h < mapDescription.size(); ++h)
  {
    auto const& line = mapDescription[h];
    tmpPosRect.y = h * TILESIZE;
    int wend = std::min(static_cast<int>((mapPos+WIDTH)/TILESIZE + 1), static_cast<int>(line.size()));
    int wstart = std::max(mapPos/TILESIZE-1,0);

    for (int w=wstart; w < wend; ++w)
    {
      int desc = line[w];
      tmpPosRect.x = w * TILESIZE - mapPos;
      
      if (desc == 0)
        continue;

      tmpTileRect.x = (desc-1) * TILESIZE;
      SDL_BlitSurface(blocksImage.get(), &tmpTileRect, screen, &tmpPosRect);
    }
  }
}

CollissionInfo GameMap::checkCollision(Object const& object)
{
  CollissionInfo collisionResult;
  SDL_Rect const& objPos = object.getPosition();

  if (objPos.y > HEIGHT)
    collisionResult.fallen = true;

  if (objPos.x < mapPos || objPos.x > mapPos+WIDTH)
    collisionResult.outOfCamera = true;

  int minh = std::max(objPos.y/TILESIZE-1, 0);
  int maxh = std::min(static_cast<int>((objPos.y+objPos.h)/TILESIZE+1), static_cast<int>(mapDescription.size()));
  int minw = std::max(objPos.x/TILESIZE-1, 0);
  // possible exception if mapDescription is empty
  int maxw = std::min(static_cast<int>((objPos.x+objPos.w)/TILESIZE+1), static_cast<int>(mapDescription[0].size()));

  for (int h = minh; h < maxh; ++h)
  {
    auto const& line = mapDescription[h];
    for (int w = minw; w < maxw; ++ w)
    {
      if (mapDescription[h][w] == 0)
        continue;

      SDL_Rect tmpRect{static_cast<Sint16>(w*TILESIZE), static_cast<Sint16>(h*TILESIZE), TILESIZE, TILESIZE};
      if (rectCollision(objPos, tmpRect))
      {
        if (w == goal.first && h == goal.second)
          collisionResult.treasure = true;

        if (objPos.y+objPos.h >= tmpRect.y && objPos.x+objPos.w > tmpRect.x && objPos.x < tmpRect.x+tmpRect.w && objPos.y < tmpRect.y)
          collisionResult.down = true;
        
        if (objPos.y <= tmpRect.y+tmpRect.h && objPos.x+objPos.w > tmpRect.x && objPos.x < tmpRect.x+tmpRect.w && objPos.y > tmpRect.y)
          collisionResult.up = true;

        if (objPos.x+objPos.w >= tmpRect.x && objPos.y+objPos.h > tmpRect.y && objPos.y < tmpRect.y+tmpRect.h &&  objPos.x < tmpRect.x)
          collisionResult.right = true;

        if (objPos.x <= tmpRect.x+tmpRect.w && objPos.y+objPos.h > tmpRect.y && objPos.y < tmpRect.y+tmpRect.h &&  objPos.x > tmpRect.x)
          collisionResult.left = true;
      }
    }
  }

  return collisionResult;
}