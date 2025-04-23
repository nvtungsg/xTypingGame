#pragma once

#include "SDL.h"

struct Bomb {
	int x, y;
	int radius;
	int explosionRadius;
	int time;
	int explodedTime;
	bool hasExploded = false;
};

void Bomb_Run();
void Bomb_Draw(SDL_Renderer* renderer);