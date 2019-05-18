#pragma once

#include "Player.h"
#include "Enemy.h"
#include "GameMap.h"

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#include <vector>
#include <optional>

class GameManager
{
public:
  GameManager();
  ~GameManager();

  void runGame();

private:
  void resetMap();
  GAME_STATE runMap();
  GAME_STATE popMenu(bool inGame = false);
  bool handleCollisions(DirectionInfo const& playerDirection);

  SDL_Surface* screen;
  TTF_Font* font;
  std::shared_ptr<Mix_Chunk> touch;
  std::shared_ptr<Mix_Music> music;

  unsigned int menuSize;
  SurfacePtrVec menusOptions;
  std::vector<SDL_Rect> menuPositions;

  SurfacePtrVec resultOptions;
  SDL_Rect resultPosition;
  std::optional<bool> gameResult;

  bool gameStarted;
  GameMapPtr gamemap;
  PlayerPtr player;
  BulletPtrVec activeBullets;
  EnemyPtrVec enemies;
};