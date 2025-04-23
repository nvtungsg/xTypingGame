#pragma once
#include "SDL_render.h"
#include "bomb.h"
#include "laser.h"
#include "rocks.h"

struct Player {
	int x, y;

	int speed;

	int size;
};

void Player_Init();
void Player_Draw(SDL_Renderer* renderer);
Player* GetPlayer();

bool Player_IsColliedWith(Bomb* bomb);
bool Player_IsColliedWith(Laser* laser);
bool Player_IsColliedWith(Rock* rock);
bool Player_IsAtBorder();