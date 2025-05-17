#include <SDL.h>
#include <SDL_ttf.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include "laser.h"
#include "constant.h"
using namespace std;

const int LASER_WIDTH = 30;
const int CHARGE_TIME = 1500;
struct Laser {
    int x, y, huong;
    Uint32 spawnTime;
    bool fired = false;
    Uint32 firedTime = 0; // thời điểm bắt đầu bắn
};
std::vector<Laser> lasers;
    Uint32 lastSpawn = SDL_GetTicks();
 // Spawn tia laser mới mỗi 2s
void ObsLaser(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect& player, bool& quit){
    srand(time(0));

    // Spawn tia laser mới mỗi 2s
    if (SDL_GetTicks() - lastSpawn > 2000) {
        lastSpawn = SDL_GetTicks();
        Laser l;
        l.x = rand() % (SCREEN_WIDTH - LASER_WIDTH);
        l.y = rand() % (SCREEN_HEIGHT - LASER_WIDTH);
        l.spawnTime = SDL_GetTicks();
        l.huong = rand() % 2; // 0: ngang, 1: dọc  ← GÁN Ở ĐÂY ✅
        lasers.push_back(l);
    }

    Uint32 now = SDL_GetTicks();

    // Cập nhật trạng thái bắn
    for (auto& l : lasers) {
        if (!l.fired && now - l.spawnTime >= CHARGE_TIME) {
            l.fired = true;
            l.firedTime = now;
        }
    }

    // Vẽ laser
    for (auto& l : lasers) {
        SDL_Rect laserRect;
        if (l.huong)  // 1 = dọc
            laserRect = { l.x, 0, LASER_WIDTH, SCREEN_HEIGHT };
        else          // 0 = ngang
            laserRect = { 0, l.y, SCREEN_WIDTH, LASER_WIDTH };

        if (!l.fired)
            SDL_SetRenderDrawColor(renderer, 255, 255, 102, 255); // vàng mờ
        else
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   // đỏ mạnh

        SDL_RenderFillRect(renderer, &laserRect);
    }

    // Xoá laser đã tồn tại quá lâu
    lasers.erase(
        remove_if(lasers.begin(), lasers.end(),
            [now](const Laser& l) {
                return l.fired && (now - l.firedTime >= 1500);
            }),
        lasers.end()
    );

    // Kiểm tra va chạm
    for (auto& l : lasers) {
        if (l.fired) {
            SDL_Rect laserRect;
            if (l.huong)
                laserRect = { l.x, 0, LASER_WIDTH, SCREEN_HEIGHT };
            else
                laserRect = { 0, l.y, SCREEN_WIDTH, LASER_WIDTH };

            if (SDL_HasIntersection(&player, &laserRect)) {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game Over", "Bạn bị trúng tia laser!", window);
                quit = true;
            }
        }
    }
}
