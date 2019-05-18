#pragma once

#include "Commons.h"
#include "Object.h"

class GameMap;
using GameMapPtr = std::shared_ptr<GameMap>;

class GameMap
{
public:
  explicit GameMap(SDL_Surface* screen);

  void render();
  CollissionInfo checkCollision(Object const&);
  void updateCamera(SDL_Rect const&);
  Sint16 cameraOffset() const;

  MapPoint const& getPlayerStartPosition();
  MapPointVec const& getEnemiesStartPosition();
private:
  void loadMap();

  MapDescription mapDescription;
  SDL_Surface* screen;
  SurfacePtr blocksImage;
  SurfacePtr backgroundImage;

  const Sint16 backLen;
  Sint16 mapPos;

  MapPoint goal;
  MapPoint playerTile;
  MapPointVec enemyTiles;
};