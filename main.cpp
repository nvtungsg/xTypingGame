#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "constant.h"
#include "player.h"
#include "laser.h"
#include "rocks.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

bool running = true;

Player player;

void ProcessEvent() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
		case SDL_QUIT:
			running = false;
			break;
		default:
			break;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_W]) player.y -= player.speed;
    if (keystates[SDL_SCANCODE_S]) player.y += player.speed;
    if (keystates[SDL_SCANCODE_A]) player.x -= player.speed;
    if (keystates[SDL_SCANCODE_D]) player.x += player.speed;

	if (player.x < SIZE_DEFAULT / 2) player.x = SIZE_DEFAULT / 2;
	if (player.y < SIZE_DEFAULT / 2) player.y = SIZE_DEFAULT / 2;
	if (player.x > SCREEN_WIDTH - SIZE_DEFAULT / 2) player.x = SCREEN_WIDTH - SIZE_DEFAULT / 2;
	if (player.y > SCREEN_HEIGHT - SIZE_DEFAULT / 2) player.y = SCREEN_HEIGHT - SIZE_DEFAULT / 2;
}

void Render() {
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	
	//player

	SDL_Rect playerLoc = { player.x - SIZE_DEFAULT / 2, player.y - SIZE_DEFAULT / 2, SIZE_DEFAULT, SIZE_DEFAULT };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &playerLoc);

    Laser_Draw(gRenderer);
	Rock_Draw(gRenderer);

	SDL_RenderPresent(gRenderer);
}

void LoadMedia() {

}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    gWindow = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

    LoadMedia();

	player.x = MIDDLE_X;
	player.y = MIDDLE_Y;
	player.speed = SPEED_DEFAULT;

    while (running) {
        ProcessEvent();
        Laser_Run();
		Rock_Run();
        Render();
        SDL_Delay(10);
    }

    return 0;
}