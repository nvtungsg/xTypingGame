#include "rocks.h"
#include "SDL.h"
#include "constant.h"
#include "player.h"
#include "SDL_image.h"
#include "sound.h"
#include <vector>
#include <algorithm>

using namespace std;

struct Explosion {
    int x, y;
    int size = 100;
    Uint32 startTime;
    int duration = 500;
};

vector<Rock> rocks;
vector<Explosion> explosions;
SDL_Texture* rockTexture = nullptr;
SDL_Texture* explosionTexture = nullptr;

int lastRockSpawnTime = SDL_GetTicks();

bool Rock_LoadTexture(SDL_Renderer* renderer) {
    // Load rock
    SDL_Surface* surface = IMG_Load("rock.png");
    if (!surface) {
        SDL_Log("Không tải được ảnh rock.png: %s", IMG_GetError());
        return false;
    }
    rockTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Load explosion
    SDL_Surface* exSurface = IMG_Load("rocketex.png");
    if (!exSurface) {
        SDL_Log("Không tải được ảnh rocketex.png: %s", IMG_GetError());
        return false;
    }
    explosionTexture = SDL_CreateTextureFromSurface(renderer, exSurface);
    SDL_FreeSurface(exSurface);

    if (rockTexture) SDL_SetTextureBlendMode(rockTexture, SDL_BLENDMODE_BLEND);
    if (explosionTexture) SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_BLEND);

    return rockTexture && explosionTexture;
}

void Rock_Cleanup() {
    if (rockTexture) {
        SDL_DestroyTexture(rockTexture);
        rockTexture = nullptr;
    }
    if (explosionTexture) {
        SDL_DestroyTexture(explosionTexture);
        explosionTexture = nullptr;
    }
}

void Rock_Run() {
    int now = SDL_GetTicks();
    if (now - lastRockSpawnTime > 2000) {
        float chance = rand() % 100;
        lastRockSpawnTime = now;
        if (chance > 20) {
            int rocksToSpawn = 1 + rand() % 3; // sinh 1 đến 3 quả tên lửa
            for (int i = 0; i < rocksToSpawn; i++) {
                Rock rock;
                rock.size = 120;
                rock.speed = 5;
                rock.x = rand() % (SCREEN_WIDTH - rock.size);
                rock.y = rand() % 50;
                rock.texture = rockTexture;
                rocks.push_back(rock);
            }
        }
    }

    for (size_t i = 0; i < rocks.size(); ) {
        Rock* rock = &rocks[i];
        rock->y += rock->speed;

        bool removed = false;

        // Chạm đáy
        if (rock->y > SCREEN_HEIGHT) {
            explosions.push_back({rock->x, SCREEN_HEIGHT - rock->size / 2, rock->size, (Uint32)SDL_GetTicks()});
            playExplosionSound();  // 🔊
            rocks.erase(rocks.begin() + i);
            removed = true;
        }

        // Chạm người chơi
        if (!removed && Player_IsCollidedWithRock({*rock})) {
            explosions.push_back({rock->x, rock->y, rock->size, (Uint32)SDL_GetTicks()});
            playExplosionSound();  // 🔊
            GetPlayer()->isAlive = false;
            rocks.erase(rocks.begin() + i);
            removed = true;
        }

        if (!removed) i++;
    }

    // Xóa hiệu ứng nổ cũ
    Uint32 current = SDL_GetTicks();
    explosions.erase(remove_if(explosions.begin(), explosions.end(),
        [current](Explosion& ex) {
            return current - ex.startTime > (Uint32)ex.duration;
        }), explosions.end());
}


void Rock_Draw(SDL_Renderer* renderer) {
    for (auto& rock : rocks) {
        SDL_Rect dstRect = { rock.x - rock.size / 2, rock.y - rock.size / 2, rock.size, rock.size };
        if (rock.texture) {
            SDL_RenderCopy(renderer, rock.texture, nullptr, &dstRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }

    for (auto& ex : explosions) {
        SDL_Rect exRect = { ex.x - ex.size / 2, ex.y - ex.size / 2, ex.size, ex.size };
        SDL_RenderCopy(renderer, explosionTexture, nullptr, &exRect);
    }
}

void Rock_Reset() {
    rocks.clear();
    explosions.clear();
}
