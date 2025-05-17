#ifndef LASER__H
#define LASER__H
#include <SDL.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#pragma once
using namespace std;
void ObsLaser(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect& player, bool& quit);

#endif // LASER__H
