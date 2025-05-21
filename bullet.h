#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>

struct Bullet {
    float x, y;
    float speedX, speedY;
    int width, height;
    bool active;
    bool homing; // true nếu là đạn đuổi theo player
    float angle; // dùng cho đạn quay tròn
};

void Bullet_Init();
void Bullet_Reset();
void Bullet_ShootMultiple();
void Bullet_Run();
void Bullet_Draw(SDL_Renderer* renderer);
void BulletBoss_Shoot();

#endif
