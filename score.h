#ifndef SCORE_H
#define SCORE_H

#include <SDL.h>

void Score_Init(SDL_Renderer* renderer);
void Score_Update(float deltaTime);
void Score_Render();
void Score_AddTime(float deltaTime);
void Score_Reset();
int Score_GetCurrent();
int Score_GetRecord();
void Score_Cleanup();
void Score_SetRecord(int newRecord);

#endif
