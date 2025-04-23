#pragma once

#include "SDL.h"


struct Laser {
	int x, y;
	int huong;
	int width;
	int time;
	int fireTime;
	bool hasFired = false;
};

void Laser_Run();
void Laser_Draw(SDL_Renderer* renderer);