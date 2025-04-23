#include "rocks.h"
#include "SDL.h"
#include "constant.h"
#include <vector>

using namespace std;

vector<Rock> rocks;

int lastRockSpawnTime = SDL_GetTicks();

void Rock_Run() {
    int now = SDL_GetTicks();
    if (now - lastRockSpawnTime > 2000) {
        lastRockSpawnTime = now;
        Rock rock;

		rock.size = max(25, rand() % 40);
		rock.speed = 10;
		rock.x = rand() % (SCREEN_WIDTH - rock.size);
		rock.y = rand() % 50;
        rocks.push_back(rock);
    }


	for (int i = 0; i < rocks.size(); i++) {
		Rock* rock = &rocks[i];
		rock->y += rock->speed;
	}
}

void Rock_Draw(SDL_Renderer* renderer) {
    int now = SDL_GetTicks();

	for (auto& rock : rocks) {
		SDL_Rect rockRect = { rock.x - rock.size / 2, rock.y - rock.size / 2, rock.size, rock.size };
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rockRect);
	}
}