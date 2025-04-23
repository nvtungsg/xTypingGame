#include "laser.h"
#include "SDL.h"
#include "constant.h"
#include <vector>

using namespace std;

vector<Laser> lasers;

int lastSpawnTime = SDL_GetTicks();

void Laser_Run() {
    int now = SDL_GetTicks();
    if (now - lastSpawnTime > 2000) {
        lastSpawnTime = now;
        Laser l;
        l.x = rand() % (SCREEN_WIDTH - LASER_WIDTH);
        l.y = rand() % (SCREEN_HEIGHT - LASER_WIDTH);
        l.time = now;
        l.huong = rand() % 2;
        lasers.push_back(l);
    }

    for (auto& laser : lasers) {
		if (!laser.hasFired && now - laser.time >= 200) {
			laser.hasFired = true;
			laser.fireTime = now;
        }
    }

}