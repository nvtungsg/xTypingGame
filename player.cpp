#include "player.h"
#include "constant.h"
#include "SDL.h"
#include "cmath";

Player player;

void Player_Init() {
	player.x = SCREEN_WIDTH / 2;
	player.y = SCREEN_HEIGHT / 2;
	player.speed = 5;
	player.size = SIZE_DEFAULT;
    player.isAlive = true; // người chơi bắt đầu còn sống

}

void Player_Draw(SDL_Renderer* renderer) {
    if (!player.isAlive) return; // nếu chết thì ko vẽ thêm gì nữa
	SDL_Rect playerLoc = { player.x - player.size / 2, player.y - player.size / 2, player.size, player.size };
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &playerLoc);
}

Player* GetPlayer() {
	return &player;
}

bool Player_IsColliedWith(Bomb* bomb) {
	return (std::abs(player.x - bomb->x) * 2 < (player.size + bomb->explosionRadius) &&
		std::abs(player.y - bomb->y) * 2 < (player.size + bomb->explosionRadius));

}

bool Player_IsAtBorder() {
	return player.x < SIZE_DEFAULT / 2 || player.y < SIZE_DEFAULT / 2 || player.x > SCREEN_WIDTH - SIZE_DEFAULT / 2 || player.y > SCREEN_HEIGHT - SIZE_DEFAULT / 2;
}
