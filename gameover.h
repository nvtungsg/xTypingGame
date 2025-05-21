#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SDL.h>

void GameOver_Start();
void GameOver_Update();
void GameOver_Render(SDL_Renderer* renderer);
bool GameOver_IsPlayAgainClicked(SDL_Point p);
bool GameOver_IsBackToMenuClicked(SDL_Point p);

#endif
