#include "bomb.h"
#include "menu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "sound.h"         // Thêm dòng này để gọi âm thanh
#include "constant.h"
#include "player.h"
#include <vector>
#include <algorithm>

using namespace std;

int lastBombSpawnTime = SDL_GetTicks();
vector<Bomb> bombs;

SDL_Texture* bombImage = NULL;
SDL_Texture* explosionImage = NULL;

bool Bomb_LoadTextures(SDL_Renderer* renderer) {
    bombImage = IMG_LoadTexture(renderer, "bomb.png");
    if (!bombImage) {
        SDL_Log("Failed to load bomb.png: %s", IMG_GetError());
        return false;
    }

    explosionImage = IMG_LoadTexture(renderer, "explosion.png");
    if (!explosionImage) {
        SDL_Log("Failed to load explosion.png: %s", IMG_GetError());
        return false;
    }

    return true;
}

void Bomb_Run() {
    int now = SDL_GetTicks();
    if (now - lastBombSpawnTime > 5000) {
        float chance = rand() % 100;
        lastBombSpawnTime = now;
        if (chance > 40) {
            Bomb bomb;
            bomb.radius = 60;
            bomb.explosionRadius = bomb.radius * 3;
            Player* player = GetPlayer();

            if (Player_IsAtBorder()) {
                bomb.x = player->x;
                bomb.y = player->y;
            } else {
                bool isNegativeX = rand() % 2;
                bool isNegativeY = rand() % 2;
                int bombX = rand() % 50;
                if (isNegativeX) bombX *= -1;
                int bombY = rand() % 50;
                if (isNegativeY) bombY *= -1;
                bomb.x = player->x + bombX;
                bomb.y = player->y + bombY;
            }

            bomb.time = now;
            bombs.push_back(bomb);
        }
    }

    for (size_t i = 0; i < bombs.size(); i++) {
        Bomb* bomb = &bombs[i];
        if (!bomb->hasExploded && now - bomb->time >= BOMB_WAIT_TIME) {
            Player* player = GetPlayer();
            if (Player_IsCollidedWithBom(bomb)) {
                SDL_Log("Player bị bom đánh!");
                player->isAlive = false;
            }

            // Thêm hiệu ứng âm thanh nổ
            if (bombExplosionSound != nullptr) {
                Mix_PlayChannel(-1, bombExplosionSound, 0);
            }

            bomb->hasExploded = true;
            bomb->explodedTime = now;
        }

        if (bomb->hasExploded && now - bomb->explodedTime >= BOMB_DURATION + BOMB_FADE_TIME) {
            bombs.erase(bombs.begin() + i);
            i--;
        }
    }
}

void Bomb_Draw(SDL_Renderer* renderer) {
    int now = SDL_GetTicks();
    for (auto& bomb : bombs) {
        int radius = bomb.hasExploded ? bomb.explosionRadius : bomb.radius;
        SDL_Rect dst = { bomb.x - radius / 2, bomb.y - radius / 2, radius, radius };

        if (!bomb.hasExploded) {
            if (bombImage) SDL_RenderCopy(renderer, bombImage, NULL, &dst);
        } else {
            if (explosionImage) {
                if (now - bomb.explodedTime < BOMB_DURATION) {
                    SDL_RenderCopy(renderer, explosionImage, NULL, &dst);
                } else {
                    // Hiệu ứng mờ dần sau khi nổ
                    int fadingTime = now - bomb.explodedTime - BOMB_DURATION;
                    int alpha = 255 - ((float)fadingTime / BOMB_FADE_TIME * 255);
                    SDL_SetTextureAlphaMod(explosionImage, alpha);
                    SDL_RenderCopy(renderer, explosionImage, NULL, &dst);
                    SDL_SetTextureAlphaMod(explosionImage, 255); // reset lại alpha
                }
            }
        }
    }
}

void Bomb_Reset() {
    bombs.clear(); // nếu dùng vector
}
