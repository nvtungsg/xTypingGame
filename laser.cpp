#include "laser.h"
#include "SDL.h"
#include "constant.h"
#include <vector>
#include <player.h>

using namespace std;

vector<Laser> lasers;

int lastSpawnTime = SDL_GetTicks();

void Laser_Run() {
    int now = SDL_GetTicks();
    if (now - lastSpawnTime > 2000) {
		float chance = rand() % 100;
        lastSpawnTime = now;
        if (chance > 40) {
            Laser laser;
            laser.width = max(20, rand() % 40);
            Player* player = GetPlayer();
            if (Player_IsAtBorder()) {
                laser.x = player->x;
                laser.y = player->y;
            }
            else {

                SDL_Log("Random laser: %d", rand());
                bool isNegativeX = rand() % 2;
                bool isNegativeY = rand() % 2;

                int laserX = rand() % 50;
                if (isNegativeX) laserX *= -1;
                int laserY = rand() % 50;
                if (isNegativeY) laserY *= -1;

                SDL_Log("laserX: %d, laserY: %d", laserX, laserY);

                laser.x = player->x + laserX;
                laser.y = player->y + laserY;
            }
            laser.time = now;
            laser.huong = rand() % 2;
            lasers.push_back(laser);
        }
    }

	for (int i = 0; i < lasers.size(); i++) {
		Laser* laser = &lasers[i];
        if (!laser->hasFired && now - laser->time >= LASER_WAIT_TIME) {
            laser->hasFired = true;
            laser->fireTime = now;
            continue;
        }

        if (laser->hasFired && now - laser->fireTime >= LASER_DURATION + LASER_FADE_TIME) {
            lasers.erase(lasers.begin() + i);
            i--;
            continue;
        }
    }
}

void Laser_Draw(SDL_Renderer* renderer) {
	int now = SDL_GetTicks();
    for (auto& laser : lasers) {
        SDL_Rect laserRect;
        if (laser.huong == 1) {
            laserRect = { laser.x - laser.width / 2, 0, laser.width, SCREEN_HEIGHT };
        }
        else {
            laserRect = { 0, laser.y - laser.width / 2, SCREEN_WIDTH, laser.width };
        }

        if (!laser.hasFired) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 102, 255);
        }
        else {
            if (now - laser.fireTime < LASER_DURATION) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else{
				int fadingTime = now - laser.fireTime - LASER_DURATION;
                int alpha = 255 - ((float)fadingTime / LASER_FADE_TIME * 255);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);
            }
        }

		SDL_RenderFillRect(renderer, &laserRect);
    }
}