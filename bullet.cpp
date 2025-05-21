#include "bullet.h"
#include "player.h"
#include "sound.h"
#include "score.h"
#include "constant.h"
#include <SDL.h>
#include <math.h>
#include <stdlib.h> // cho rand()

#define MAX_BULLETS 50
#define BULLET_SPEED 4.5f
#define BULLET_SIZE 12
#define PI 3.14159265f

static Bullet bullets[MAX_BULLETS];
static int bulletCount = 0;
static int lastScoreShoot = 0;
static int lastBossScore = 0;

// Góc bắn của Bullet Boss để tạo hiệu ứng quay vòng
static float bulletBossAngle = 0.0f;

// Khởi tạo tất cả đạn thành inactive
void Bullet_Init() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    bulletCount = 0;
    lastScoreShoot = 0;
    lastBossScore = 0;
}

void Bullet_Reset() {
    Bullet_Init();
}

// Bắn 5 viên đạn thẳng từ trên xuống đều đều
void Bullet_ShootMultiple() {
    int num = 5;
    int gap = SCREEN_WIDTH / (num + 1);

    for (int i = 0; i < num; i++) {
        bullets[i].x = gap * (i + 1);
        bullets[i].y = -BULLET_SIZE;
        bullets[i].speedX = 0;
        bullets[i].speedY = BULLET_SPEED;
        bullets[i].width = BULLET_SIZE;
        bullets[i].height = BULLET_SIZE;
        bullets[i].active = true;
        bullets[i].homing = false;
        bullets[i].angle = 0.0f;
    }
    bulletCount = num;
    playBulletSound();

}

// Bắn 5 viên đạn homing (đuổi theo người chơi)
void ShootHomingBullet() {
    for (int i = 5; i < 10; i++) {
        bullets[i].x = rand() % (SCREEN_WIDTH - BULLET_SIZE);
        bullets[i].y = -BULLET_SIZE;
        bullets[i].width = BULLET_SIZE;
        bullets[i].height = BULLET_SIZE;
        bullets[i].active = true;
        bullets[i].homing = true;
        bullets[i].angle = 0.0f;
    }
      playBulletSound();
}

// Bắn đạn theo hình tròn (16 viên)
void ShootCircularBullets() {
    Player* player = GetPlayer();
    float centerX = player->x;
    float centerY = -30.0f;

    for (int i = 10; i < 26; i++) {
        float angle = 2.0f * PI * (i - 10) / 16.0f;
        bullets[i].x = centerX;
        bullets[i].y = centerY;
        bullets[i].speedX = cosf(angle) * BULLET_SPEED;
        bullets[i].speedY = sinf(angle) * BULLET_SPEED;
        bullets[i].width = BULLET_SIZE;
        bullets[i].height = BULLET_SIZE;
        bullets[i].active = true;
        bullets[i].homing = false;
        bullets[i].angle = angle;
    }
      playBulletSound();
}

// Bắn đạn boss 8 viên theo vòng tròn ở giữa màn hình
void BulletBoss_Shoot() {
    const int NUM_BOSS_BULLETS = 8;
    const int BOSS_X = SCREEN_WIDTH / 2;
    const int BOSS_Y = SCREEN_HEIGHT / 2;

    float angleStep = 360.0f / NUM_BOSS_BULLETS;

    for (int i = 0; i < NUM_BOSS_BULLETS; i++) {
        float angle = bulletBossAngle + i * angleStep;
        float rad = angle * PI / 180.0f;

        // Tìm slot đạn trống từ vị trí 26 trở đi
        int idx = -1;
        for (int j = 26; j < MAX_BULLETS; j++) {
            if (!bullets[j].active) {
                idx = j;
                break;
            }
        }
        if (idx == -1) break; // không còn slot đạn boss

        bullets[idx].x = (float)BOSS_X;
        bullets[idx].y = (float)BOSS_Y;
        bullets[idx].speedX = cosf(rad) * BULLET_SPEED;
        bullets[idx].speedY = sinf(rad) * BULLET_SPEED;
        bullets[idx].width = BULLET_SIZE;
        bullets[idx].height = BULLET_SIZE;
        bullets[idx].active = true;
        bullets[idx].homing = false;
        bullets[idx].angle = rad;
    }

    bulletBossAngle += 5.0f; // quay dần góc
    if (bulletBossAngle >= 360.0f) bulletBossAngle -= 360.0f;
    playBulletSound();
}

void Bullet_Run() {
    int currentScore = Score_GetCurrent();

    // Bắn 5 viên khi điểm là bội số của 500, tránh bắn liên tục cùng điểm
    if (currentScore > 0 && currentScore % 500 == 0 && lastScoreShoot != currentScore) {
        Bullet_ShootMultiple();
        lastScoreShoot = currentScore;
    }

    // Bắn đạn boss khi điểm là bội số của 1250, tránh bắn liên tục cùng điểm
    if (currentScore > 0 && currentScore % 1250 == 0 && lastBossScore != currentScore) {
        BulletBoss_Shoot();
        lastBossScore = currentScore;
    }

    bulletCount = 0;

    Player* player = GetPlayer();

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        // Nếu đạn homing thì điều chỉnh hướng bay về phía người chơi
        if (bullets[i].homing) {
            float dx = player->x - bullets[i].x;
            float dy = player->y - bullets[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 0.01f) {
                bullets[i].speedX = BULLET_SPEED * dx / dist;
                bullets[i].speedY = BULLET_SPEED * dy / dist;
            }
        }

        // Cập nhật vị trí
        bullets[i].x += bullets[i].speedX;
        bullets[i].y += bullets[i].speedY;

        // Tắt đạn khi ra ngoài màn hình với vùng đệm 50px
        if (bullets[i].x < -50 || bullets[i].x > SCREEN_WIDTH + 50 || bullets[i].y > SCREEN_HEIGHT + 50) {
            bullets[i].active = false;
        } else {
            bulletCount++;
        }

        // Kiểm tra va chạm đạn với người chơi
        SDL_Rect rectBullet = {
            (int)bullets[i].x, (int)bullets[i].y,
            bullets[i].width, bullets[i].height
        };
        SDL_Rect rectPlayer = {
            (int)(player->x - SIZE_DEFAULT / 2),
            (int)(player->y - SIZE_DEFAULT / 2),
            SIZE_DEFAULT, SIZE_DEFAULT
        };

        if (SDL_HasIntersection(&rectBullet, &rectPlayer)) {
            player->isAlive = false;
        }
    }
}

void Bullet_Draw(SDL_Renderer* renderer) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        Uint8 r = 255;
        Uint8 g = 50 + rand() % 100;
        Uint8 b = 50 + rand() % 100;
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        SDL_FRect bulletRect = {
            bullets[i].x,
            bullets[i].y,
            (float)bullets[i].width,
            (float)bullets[i].height
        };
        SDL_RenderFillRectF(renderer, &bulletRect);
    }
}
