#pragma once
#ifndef BOMB_H
#define BOMB_H
#include "SDL.h"

struct Bomb {
	int x, y;
	int radius;
	int explosionRadius;
	int time;
	int explodedTime;
	bool hasExploded = false;

    SDL_Texture* imageBeforeExplode = nullptr;
    SDL_Texture* imageAfterExplode = nullptr;
};

void Bomb_Run();
void Bomb_Draw(SDL_Renderer* renderer);

bool Bomb_LoadTextures(SDL_Renderer* renderer);
extern SDL_Texture* bombImage;
extern SDL_Texture* explosionImage;

#endif

