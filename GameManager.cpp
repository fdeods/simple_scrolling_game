#include "GameManager.h"

#include "Commons.h"

#include <tuple>
#include <set>
#include <algorithm>

namespace {
using RectVec = std::vector<SDL_Rect>;
void surfaceDeleter(SDL_Surface* surface)
{
  SDL_FreeSurface(surface);
}

SDL_Color WHITE{0xff, 0xff, 0xff}, RED{0xff, 0x00, 0x00}, GREEN{0x00, 0xff, 0x00};

void prepareMenuOptions(TTF_Font* font, SDL_Surface* screen, SurfacePtrVec& menusOptions, RectVec& positions, unsigned int& menuSize)
{
menusOptions.push_back(SurfacePtr(TTF_RenderText_Solid(font,"For the treasure!", WHITE), surfaceDeleter));
menusOptions.push_back(SurfacePtr(TTF_RenderText_Solid(font,"For the treasure!", RED), surfaceDeleter));
menusOptions.push_back(SurfacePtr(TTF_RenderText_Solid(font,"Exit", WHITE), surfaceDeleter));
menusOptions.push_back(SurfacePtr(TTF_RenderText_Solid(font,"Exit", RED), surfaceDeleter));  

menuSize = menusOptions.size()/2;
positions.push_back(SDL_Rect{static_cast<Sint16>((screen->clip_rect.w - menusOptions[0]->clip_rect.w)/2u), 
                             static_cast<Sint16>((screen->clip_rect.h - menusOptions[0]->clip_rect.h)/2u), 
                             menusOptions[0]->clip_rect.w, 
                             menusOptions[0]->clip_rect.h});
positions.push_back(SDL_Rect{static_cast<Sint16>((screen->clip_rect.w - menusOptions[2]->clip_rect.w)/2u), 
                             static_cast<Sint16>((screen->clip_rect.h + menusOptions[2]->clip_rect.h)/2u), 
                             menusOptions[2]->clip_rect.w, 
                             menusOptions[2]->clip_rect.h});
}

void prepareResultOptions(TTF_Font* font, SDL_Surface* screen, SDL_Rect& resultPosition, SurfacePtrVec& resultOptions)
{
  resultOptions.push_back(SurfacePtr(TTF_RenderText_Blended(font, "You've won", GREEN), surfaceDeleter));
  resultOptions.push_back(SurfacePtr(TTF_RenderText_Blended(font, "You've lost", RED), surfaceDeleter));
  resultPosition = SDL_Rect{
    static_cast<Sint16>((screen->clip_rect.w - resultOptions[0]->clip_rect.w)/2u), 
    50,
    resultOptions[0]->clip_rect.w, 
    resultOptions[0]->clip_rect.h
  };
}
}

GameManager::GameManager()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();
  Mix_Init(MIX_INIT_MP3);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
  font = TTF_OpenFont(FONTFILE, 60);
  Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
  SDL_WM_SetCaption("Simple scrolling game", NULL);
  prepareMenuOptions(font, screen, menusOptions, menuPositions, menuSize);
  prepareResultOptions(font, screen, resultPosition, resultOptions);
  gamemap = std::make_shared<GameMap>(screen);
}

GameManager::~GameManager()
{
  Mix_CloseAudio();
  Mix_Quit();
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
}

void GameManager::resetMap()
{
  MapPoint const& playerPoint = gamemap->getPlayerStartPosition();
  player = std::make_shared<Player>(screen, playerPoint.first*TILESIZE, playerPoint.second*TILESIZE);

  enemies.clear();
  for (auto const& enemy : gamemap->getEnemiesStartPosition())
    enemies.push_back(Enemy::create(screen, enemy.first*TILESIZE, enemy.second*TILESIZE));
    
  gameResult = std::nullopt;
}

namespace {
void checkFPS(Uint32 start)
{
  auto duration = SDL_GetTicks() - start;
  if (duration < 1000/FPS)
      SDL_Delay(1000/FPS - duration);
}

bool isPointInsideRect(SDL_Rect const& rect, int xPos, int yPos)
{
  return (xPos >= rect.x && xPos <= rect.x+rect.w && yPos >= rect.y && yPos <= rect.y+rect.h) ? true : false;
}
}

bool GameManager::handleCollisions(DirectionInfo const& playerDirection)
{
  // player collisions with map
  CollissionInfo collision = gamemap->checkCollision(*player);
  player->move(playerDirection, collision);

  if (collision.fallen)
  {
    gameResult = false;
    return false;
  }

  if (collision.treasure)
  {
    gameResult = true;
    return false;
  }

  for (EnemyPtr const& enemy : enemies)
    if (enemy->collision(player->getPosition()))
    {
      gameResult = false;
      return false;
    }

  std::set<EnemyPtr> enemiesToDelete;
  std::set<BulletPtr> bulletsToDelete;

  // enemy collisions with map
  for (EnemyPtr const& enemy : enemies)
  {
    CollissionInfo collision = gamemap->checkCollision(*enemy);
    if (collision.fallen)
    {
      enemiesToDelete.insert(enemy);
      continue;
    }

    enemy->move(collision);
  }

  // bullet collisions with map
  for (BulletPtr const& bullet : activeBullets)
  {
    CollissionInfo collision = gamemap->checkCollision(*bullet);
    if (collision.outOfCamera || collision.left || collision.right)
    {
      bulletsToDelete.insert(bullet);
      continue;
    }

    bullet->move();
  }

  for (BulletPtr const& bullet : activeBullets)
  {
    for (EnemyPtr const& enemy : enemies)
    {
      if (enemy->collision(bullet->getPosition()))
      {
        bulletsToDelete.insert(bullet);
        enemiesToDelete.insert(enemy);        
      }
    }
  }

  auto newBulletsEnd = std::remove_if(activeBullets.begin(), activeBullets.end(), [&](BulletPtr const& bullet){
    return std::find(bulletsToDelete.begin(), bulletsToDelete.end(), bullet) != bulletsToDelete.end();
  });
  auto newEnemiesEnd = std::remove_if(enemies.begin(), enemies.end(), [&](EnemyPtr const& enemy){
    return std::find(enemiesToDelete.begin(), enemiesToDelete.end(), enemy)!=enemiesToDelete.end();
  });

  activeBullets.erase(newBulletsEnd, activeBullets.end());
  enemies.erase(newEnemiesEnd, enemies.end());

  return true;
}

GAME_STATE GameManager::runMap()
{
  GAME_STATE gameState = CONTINUE;
  SDL_Event event;
  DirectionInfo playerDirection{0,0,0,0};
  while (gameState == CONTINUE)
  {
    auto start = SDL_GetTicks();
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          gameState = EXIT_PROGRAM;
          break;
        case SDL_KEYDOWN:
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_UP:
              playerDirection.up = true;
              break;
            case SDLK_DOWN:
              playerDirection.down = true;
              break;
            case SDLK_LEFT:
              playerDirection.left = true;
              break;
            case SDLK_RIGHT:
              playerDirection.right = true;
              break;
            case SDLK_SPACE:
              activeBullets.push_back(player->fire());
              break;
            case SDLK_ESCAPE:
              gameState = popMenu(true);
              break;
          }
          break;
        }
        case SDL_KEYUP:
        {
          switch (event.key.keysym.sym)
          {
            case SDLK_UP:
              playerDirection.up = false;
              break;
            case SDLK_DOWN:
              playerDirection.down = false;
              break;
            case SDLK_LEFT:
              playerDirection.left = false;
              break;
            case SDLK_RIGHT:
              playerDirection.right = false;
              break;
          }
        }
      }
    }

    // logic
    if (gameState == CONTINUE && !handleCollisions(playerDirection))
      gameState = EXIT;

    // render
    gamemap->updateCamera(player->getPosition());
    gamemap->render();
    const Sint16 cameraOffset = gamemap->cameraOffset();
    player->render(cameraOffset);
    for (BulletPtr const& bullet : activeBullets)
        bullet->render(cameraOffset);
    for (EnemyPtr const& enemy : enemies)
        enemy->render(cameraOffset);

    SDL_Flip(screen);

    checkFPS(start);
  }
  
  return gameState;
}

GAME_STATE GameManager::popMenu(bool inGame)
{
  bool running = true;
  int mouseXPos, mouseYPos;
  std::vector<bool> isActivated(menuSize, false);

  if (inGame)
  {
    static Uint32 const BLACKIE = SDL_MapRGB(screen->format, 0x00, 0x00, 0x01);
    SurfacePtr aplha(SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 32, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask), surfaceDeleter);
    if (aplha)
    {
      SDL_FillRect(aplha.get(), &aplha->clip_rect, BLACKIE);
      SDL_SetAlpha(aplha.get(), SDL_SRCALPHA, 129);
      SDL_BlitSurface(aplha.get(), NULL, screen, NULL);
    }
  }
  else
  {
    SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
  }

  SDL_Event event;
  while(running){
    auto start = SDL_GetTicks();
    
    while (SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          return EXIT_PROGRAM;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
            return CONTINUE;
          break;
        case SDL_MOUSEMOTION:
          mouseXPos = event.motion.x;
          mouseYPos = event.motion.y;
          for (int i=0; i < menuSize; ++i)
            isActivated[i] = isPointInsideRect(menuPositions[i], mouseXPos, mouseYPos);
          break;
        case SDL_MOUSEBUTTONDOWN:
          mouseXPos = event.motion.x;
          mouseYPos = event.motion.y;
          for (int i=0; i < menuSize; ++i)
            if (isPointInsideRect(menuPositions[i], mouseXPos, mouseYPos))
              return static_cast<GAME_STATE>(i);
          break;
        }
      }

      for (int i = 0; i < menuSize; ++i)
        SDL_BlitSurface(menusOptions[i*2 + (isActivated[i] ? 1 : 0)].get(), NULL, screen, &menuPositions[i]);

      if (gameResult)
        SDL_BlitSurface(resultOptions[(gameResult.value() ? 0 : 1)].get(), NULL, screen, &resultPosition);

      SDL_Flip(screen);

      checkFPS(start);
  }
}

void GameManager::runGame()
{
  while (true)
  {
    if (popMenu() != CONTINUE)
      break;
    
    resetMap();

    if (runMap() == EXIT_PROGRAM)
      break;
  }
}