#include "rocks.h"
#include "SDL.h"
#include "constant.h"
#include "player.h"
#include "SDL_image.h"
#include <vector>

using namespace std;

vector<Rock> rocks;
SDL_Texture* rockTexture = nullptr; // Dùng chung cho tất cả viên đá

int lastRockSpawnTime = SDL_GetTicks();

bool Rock_LoadTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("rock.png");  // Ảnh PNG có nền trong suốt
    if (!surface) {
        SDL_Log("Không tải được ảnh rock.png: %s", IMG_GetError());
        return false;
    }

    rockTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (rockTexture) {
        SDL_SetTextureBlendMode(rockTexture, SDL_BLENDMODE_BLEND); // Hỗ trợ alpha transparency
    }

    return rockTexture != nullptr;
}

// Hàm giải phóng texture khi thoát game
void Rock_Cleanup() {
    if (rockTexture) {
        SDL_DestroyTexture(rockTexture);
        rockTexture = nullptr;
    }
}

void Rock_Run() {
    int now = SDL_GetTicks();
    if (now - lastRockSpawnTime > 2000) {
        float chance = rand() % 100;
        lastRockSpawnTime = now;
        if (chance > 20) {
            Rock rock;
            rock.size = 100;
            rock.speed = 5;
            rock.x = rand() % (SCREEN_WIDTH - rock.size);
            rock.y = rand() % 50;
            rock.texture = rockTexture;
            rocks.push_back(rock);
        }
    }

    for (size_t i = 0; i < rocks.size(); i++) {
        Rock* rock = &rocks[i];
        rock->y += rock->speed;
    }

    if (Player_IsCollidedWithRock(rocks)) {
        GetPlayer()->isAlive = false;
    }
}

void Rock_Draw(SDL_Renderer* renderer) {
    for (auto& rock : rocks) {
        SDL_Rect dstRect = { rock.x - rock.size / 2, rock.y - rock.size / 2, rock.size, rock.size };
        if (rock.texture) {
            SDL_RenderCopy(renderer, rock.texture, nullptr, &dstRect); // Vẽ bằng ảnh
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Phòng khi không có ảnh
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }
}
