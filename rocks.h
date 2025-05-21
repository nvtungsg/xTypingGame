#pragma once

#include "SDL.h"

struct Rock {
    int x, y;
    int size;
    int speed;
    SDL_Texture* texture = nullptr;
};

void Rock_Run();
void Rock_Draw(SDL_Renderer* renderer);
bool Rock_LoadTexture(SDL_Renderer* renderer);
void Rock_Cleanup();
void Rock_Reset();
