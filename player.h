#pragma once
#include "SDL_render.h"
#include "bomb.h"
#include "laser.h"
#include "rocks.h"
#include <vector>

struct Player {
	int x, y;
	int speed;
	int size;
	bool isAlive;
};

void Player_Init();
void Player_Draw(SDL_Renderer* renderer);
Player* GetPlayer();
void Player_Destroy();
bool Player_LoadTexture(SDL_Renderer* renderer, const char* filePath);
bool Player_IsCollidedWithBom(Bomb* bomb);
bool Player_IsCollidedWithRock(const std::vector<Rock>& rocks);
bool Player_IsAtBorder();
bool Player_IsCollidedWithLaser(const Laser* laser);
