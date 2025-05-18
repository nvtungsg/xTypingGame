#include <SDL.h>
#include <vector>
#include <SDL_image.h>
#include <cmath>
#include "player.h"
#include "constant.h"
#include "laser.h"
#include "rocks.h"

static SDL_Texture* playerTexture = nullptr;

Player player;

void Player_Init() {
    player.x = SCREEN_WIDTH / 2;
    player.y = SCREEN_HEIGHT / 2;
    player.speed = 7.5;
    player.size = SIZE_DEFAULT;
    player.isAlive = true; // người chơi bắt đầu còn sống
}

// Hàm load ảnh player.bmp thành texture
bool Player_LoadTexture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", filePath, SDL_GetError());
        return false;
    }
    playerTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!playerTexture) {
        SDL_Log("Failed to create texture from %s: %s", filePath, SDL_GetError());
        return false;
    }
    return true;
}

// Hàm giải phóng texture khi không dùng nữa
void Player_Destroy() {
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
    }
}

void Player_Draw(SDL_Renderer* renderer) {
    if (!player.isAlive) return; // nếu chết thì ko vẽ thêm gì nữa

    SDL_Rect playerLoc = { player.x - player.size / 2, player.y - player.size / 2, player.size, player.size };

    SDL_RenderCopy(renderer, playerTexture, NULL, &playerLoc);
}

Player* GetPlayer() {
    return &player;
}

bool Player_IsCollidedWithBom(Bomb* bomb) {
    return (std::abs(player.x - bomb->x) * 2 < (player.size + bomb->explosionRadius) &&
            std::abs(player.y - bomb->y) * 2 < (player.size + bomb->explosionRadius));
}

bool Player_IsAtBorder() {
    return player.x < SIZE_DEFAULT / 2 || player.y < SIZE_DEFAULT / 2 ||
           player.x > SCREEN_WIDTH - SIZE_DEFAULT / 2 || player.y > SCREEN_HEIGHT - SIZE_DEFAULT / 2;
}

bool Player_IsCollidedWithLaser(const Laser* laser) {
    SDL_Rect playerRect = {
        player.x - player.size / 2,
        player.y - player.size / 2,
        player.size,
        player.size
    };

    SDL_Rect laserRect;
    if (laser->huong == 1) {
        laserRect = { laser->x - laser->width / 2, 0, laser->width, SCREEN_HEIGHT };
    } else {
        laserRect = { 0, laser->y - laser->width / 2, SCREEN_WIDTH, laser->width };
    }

    return SDL_HasIntersection(&playerRect, &laserRect);
}

bool Player_IsCollidedWithRock(const std::vector<Rock>& rocks) {
    Player* playerPtr = GetPlayer();
    for (const Rock& rock : rocks) {
        SDL_Rect rect1 = { playerPtr->x, playerPtr->y, playerPtr->size, playerPtr->size };
        SDL_Rect rect2 = { rock.x, rock.y, rock.size, rock.size };
        if (SDL_HasIntersection(&rect1, &rect2)) {
            return true;
        }
    }
    return false;
}
