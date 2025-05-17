#include "bomb.h"
#include "SDL.h"
#include "constant.h"
#include "player.h"
#include <vector>

using namespace std;

int lastBombSpawnTime = SDL_GetTicks();

vector<Bomb> bombs;

void Bomb_Run() {
    int now = SDL_GetTicks();
    if (now - lastBombSpawnTime > 2000) {
        float chance = rand() % 100;
        lastBombSpawnTime = now;
        if (chance > 40) {
            Bomb bomb;
            bomb.radius = max(20, rand() % 40);
			bomb.explosionRadius = bomb.radius * 4;
			Player* player = GetPlayer();
            if (Player_IsAtBorder()) {
                bomb.x = player->x;
                bomb.y = player->y;
            }
            else {

				SDL_Log("Random bomb: %d", rand());
                bool isNegativeX = rand() % 2;
                bool isNegativeY = rand() % 2;

                int bombX = rand() % 50;
                if (isNegativeX) bombX *= -1;
                int bombY = rand() % 50;
                if (isNegativeY) bombY *= -1;

				SDL_Log("bombX: %d, bombY: %d", bombX, bombY);

                bomb.x = player->x + bombX;
                bomb.y = player->y + bombY;
            }
            bomb.time = now;
			bombs.push_back(bomb);
        }
    }

    for (int i = 0; i < bombs.size(); i++) {
		Bomb* bomb = &bombs[i];
        if (!bomb->hasExploded && now - bomb->time >= BOMB_WAIT_TIME) {
			Player* player = GetPlayer();
            if (Player_IsCollidedWithBom(bomb)) {
                SDL_Log("bi bom danh ne hihi");
                player ->isAlive = false;
            }
            bomb->hasExploded = true;
            bomb->explodedTime = now;
            continue;
        }

        if (bomb->hasExploded && now - bomb->explodedTime >= BOMB_DURATION + BOMB_FADE_TIME) {
            bombs.erase(bombs.begin() + i);
            i--;
            continue;
        }
    }
}

void Bomb_Draw(SDL_Renderer* renderer) {
	int now = SDL_GetTicks();
	for (auto& bomb : bombs) {
        int radius = bomb.hasExploded ? bomb.explosionRadius : bomb.radius;
		if (!bomb.hasExploded) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 102, 255);
		}
		else {
            if (now - bomb.explodedTime < BOMB_DURATION) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            else {
                int fadingTime = now - bomb.explodedTime - BOMB_DURATION;
                int alpha = 255 - ((float)fadingTime / BOMB_FADE_TIME * 255);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);
            }
		}
		SDL_Rect bombRect = { bomb.x - radius / 2, bomb.y - radius / 2, radius, radius };
		SDL_RenderFillRect(renderer, &bombRect);
	}
}
