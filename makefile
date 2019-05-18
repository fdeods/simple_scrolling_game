CC=g++-7
CFLAGS=-lSDLmain -lSDL -lSDL_ttf -lSDL_mixer -std=c++17 -I.

sgame: Commons.h Object.cpp Bullet.cpp Enemy.cpp Player.cpp GameMap.cpp GameManager.cpp main.cpp
	$(CC) -o sgame  Object.cpp Bullet.cpp Enemy.cpp Player.cpp GameMap.cpp GameManager.cpp main.cpp $(CFLAGS)
