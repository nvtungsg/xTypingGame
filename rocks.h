#pragma once

#include "SDL.h"

struct Rock {
	int x, y;
	int size;
	int speed;
};

void Rock_Run();
void Rock_Draw(SDL_Renderer* renderer);