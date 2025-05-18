#include "laser.h"
#include "SDL.h"
#include "constant.h"
#include <vector>
#include "player.h"
#include <cstdlib>

using namespace std;

vector<Laser> lasers;
int lastSpawnTime = SDL_GetTicks();

void Laser_Run() {
    int now = SDL_GetTicks();

    // Tạo laser mới mỗi 2 giây
    if (now - lastSpawnTime > LASER_WAIT_TIME) {
        float chance = rand() % 100;
        lastSpawnTime = now;

        if (chance > 40) {
            Laser laser;
            laser.width = 50;
            Player* player = GetPlayer();

            if (Player_IsAtBorder()) {
                laser.x = player->x;
                laser.y = player->y;
            } else {
                bool isNegativeX = rand() % 2;
                bool isNegativeY = rand() % 2;

                int laserX = rand() % 50;
                if (isNegativeX) laserX *= -1;
                int laserY = rand() % 50;
                if (isNegativeY) laserY *= -1;

                laser.x = player->x + laserX;
                laser.y = player->y + laserY;
            }

            laser.time = now;
            laser.huong = rand() % 2;
            laser.hasFired = false;
            lasers.push_back(laser);
        }
    }

    // Cập nhật trạng thái laser
    for (size_t i = 0; i < lasers.size(); i++) {
        Laser* laser = &lasers[i];

        if (!laser->hasFired && now - laser->time >= LASER_WAIT_TIME) {
            laser->hasFired = true;
            laser->fireTime = now;

            // Kiểm tra va chạm với người chơi
            Player* player = GetPlayer();
            if (player->isAlive && Player_IsCollidedWithLaser(laser)) {
                SDL_Log("Người chơi bị trúng laser!");
                player->isAlive = false;
            }

            continue;
        }

        if (laser->hasFired && now - laser->fireTime >= LASER_DURATION + LASER_FADE_TIME) {
            lasers.erase(lasers.begin() + i);
            i--;
            continue;
        }
    }
}

void DrawElectricEffect(SDL_Renderer* renderer, const SDL_Rect& laserRect, int segments = 8) {
    // Vẽ tia điện zigzag nằm dọc hoặc ngang tùy hướng laser
    int segmentLength;
    if (laserRect.w < laserRect.h) {
        // Laser đứng (dọc)
        segmentLength = laserRect.h / segments;
        int xCenter = laserRect.x + laserRect.w / 2;
        int prevY = laserRect.y;
        for (int i = 0; i < segments; i++) {
            int nextY = laserRect.y + (i + 1) * segmentLength;
            int offsetX = (rand() % (laserRect.w / 2)) - (laserRect.w / 4);  // lệch trái/phải ngẫu nhiên

            SDL_RenderDrawLine(renderer, xCenter + offsetX, prevY, xCenter - offsetX, nextY);
            prevY = nextY;
        }
    } else {
        // Laser ngang
        segmentLength = laserRect.w / segments;
        int yCenter = laserRect.y + laserRect.h / 2;
        int prevX = laserRect.x;
        for (int i = 0; i < segments; i++) {
            int nextX = laserRect.x + (i + 1) * segmentLength;
            int offsetY = (rand() % (laserRect.h / 2)) - (laserRect.h / 4); // lệch lên/xuống ngẫu nhiên

            SDL_RenderDrawLine(renderer, prevX, yCenter + offsetY, nextX, yCenter - offsetY);
            prevX = nextX;
        }
    }
}

void Laser_Draw(SDL_Renderer* renderer) {
    int now = SDL_GetTicks();

    for (auto& laser : lasers) {
        SDL_Rect laserRect;
        if (laser.huong == 1) {
            laserRect = { laser.x - laser.width / 2, 0, laser.width, SCREEN_HEIGHT };
        } else {
            laserRect = { 0, laser.y - laser.width / 2, SCREEN_WIDTH, laser.width };
        }

        if (!laser.hasFired) {
            // Màu trắng báo hiệu laser sắp xuất hiện (tĩnh)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &laserRect);
        } else {
            if (now - laser.fireTime < LASER_DURATION) {
                // Glow layer 1 (bên ngoài, mờ) - xanh dương nhạt
                SDL_SetRenderDrawColor(renderer, 100, 149, 237, 50);  // cornflower blue, alpha thấp
                SDL_Rect glowRect = laserRect;
                glowRect.x -= 6; glowRect.w += 12;
                glowRect.y -= 6; glowRect.h += 12;
                SDL_RenderFillRect(renderer, &glowRect);

                // Glow layer 2 (gần giữa, sáng hơn) - xanh dương đậm hơn
                SDL_SetRenderDrawColor(renderer, 30, 144, 255, 150); // dodger blue
                glowRect = laserRect;
                glowRect.x -= 3; glowRect.w += 6;
                glowRect.y -= 3; glowRect.h += 6;
                SDL_RenderFillRect(renderer, &glowRect);

                // Core laser (giữa, sắc nét) - xanh dương sáng
                SDL_SetRenderDrawColor(renderer, 0, 191, 255, 255); // deep sky blue
                SDL_RenderFillRect(renderer, &laserRect);

                // Vẽ tia điện bên trong laser - xanh dương nhạt, hơi sáng, alpha 180
                SDL_SetRenderDrawColor(renderer, 173, 216, 230, 180); // light blue
                DrawElectricEffect(renderer, laserRect);
            } else {
                int fadingTime = now - laser.fireTime - LASER_DURATION;
                int alpha = 255 - ((float)fadingTime / LASER_FADE_TIME * 255);
                if (alpha < 0) alpha = 0;

                SDL_SetRenderDrawColor(renderer, 0, 191, 255, alpha);
                SDL_RenderFillRect(renderer, &laserRect);
            }
        }
    }
}
