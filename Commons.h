#pragma once

#include <SDL/SDL.h>

#include <memory>
#include <vector>

#include <iostream>

const char* const ENEMYFILE = "assets/enemy.bmp";
const char* const BLOCKSFILE = "assets/blocks.bmp";
const char* const BACKGROUNDFILE = "assets/back.bmp";
const char* const MAPDESCFILE = "assets/map.dat";
const char* const HEROFILE = "assets/hero.bmp";
const char* const BULLETFILE = "assets/bullet.bmp";
const char* const FONTFILE = "assets/endor.ttf";

constexpr unsigned int WIDTH = 800;
constexpr unsigned int HEIGHT = 600;
constexpr unsigned int FPS = 30;
constexpr Uint16 TILESIZE = 50;
constexpr int GOALTILE = 5;
constexpr int PLAYERTILE = 6;
constexpr int ENEMYTILE = 7;
constexpr Uint16 WINDOWMIN = 100;
constexpr Uint16 WINDOWMAX = 300;

constexpr int PLAYERVX = 5;
constexpr int PLAYERVY = 8;
constexpr unsigned int PLAYERFRAMES = 6;
constexpr unsigned int PLAYERFRAMESTEP = 5;

constexpr Uint16 TEXTLEN = 40;

constexpr unsigned int BULLETVX = 15;
constexpr Uint16 BULLETSIZE = 15;

constexpr unsigned int ENEMYVX = 3;
constexpr unsigned int ENEMYVY = 5;
constexpr unsigned int ENEMYFRAMES = 2;
constexpr unsigned int ENEMYFRAMESTEP = 2;

struct CollissionInfo
{
  bool down = false;
  bool up = false;
  bool left = false;
  bool right = false;
  bool fallen = false;
  bool outOfCamera = false;
  bool treasure = false;
};

enum GAME_STATE
{
  CONTINUE,
  EXIT,
  EXIT_PROGRAM
};

using SurfacePtr = std::shared_ptr<SDL_Surface>;
using SurfacePtrVec = std::vector<SurfacePtr>;
using MapDescription = std::vector<std::vector<int>>;
using MapPoint = std::pair<int, int>;
using MapPointVec = std::vector<MapPoint>;


inline bool rectCollision(SDL_Rect const& rect1, SDL_Rect const& rect2)
{
  if (rect1.y + rect1.h < rect2.y)
      return false;
  if (rect1.y > rect2.y + rect2.h)
      return false;
  if (rect1.x + rect1.w < rect2.x)
      return false;
  if (rect1.x > rect2.x + rect2.w)
      return false;
  return true;
}

inline bool epsRectCollision(SDL_Rect const rect1, SDL_Rect const& rect2)
{
  const Uint16 epsilon = TILESIZE/10;
  if (rect1.y + rect1.h < rect2.y + epsilon)
      return false;
  if (rect1.y > rect2.y + rect2.h - epsilon)
      return false;
  if (rect1.x + rect1.w < rect2.x + epsilon)
      return false;
  if (rect1.x > rect2.x + rect2.w - epsilon)
      return false;
  return true;
}

struct DirectionInfo
{
  bool up;
  bool right;
  bool down;
  bool left;
};