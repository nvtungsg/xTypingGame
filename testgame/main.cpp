#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "player.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int MIDDLE_X = SCREEN_WIDTH / 2;
const int MIDDLE_Y = SCREEN_HEIGHT / 2;

const int SPEED_DEFAULT = 5;
const int SIZE_DEFAULT = 10;

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

	if (player.x < 0) player.x = 0;
	if (player.x > SCREEN_WIDTH - SIZE_DEFAULT / 2) player.x = SCREEN_WIDTH - SIZE_DEFAULT / 2;
	if (player.y < 0) player.y = 0;
	if (player.y > SCREEN_HEIGHT - SIZE_DEFAULT / 2) player.y = SCREEN_HEIGHT - SIZE_DEFAULT / 2;
}

void Render() {
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	
	//player

	SDL_Rect playerLoc = { player.x, player.y, 50, 50 };
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_RenderFillRect(gRenderer, &playerLoc);

	SDL_RenderPresent(gRenderer);
}

void LoadMedia() {

}

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    gWindow = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

    LoadMedia();

	player.x = MIDDLE_X;
	player.y = MIDDLE_Y;
	player.speed = SPEED_DEFAULT;

    while (running) {
        ProcessEvent();
        Render();
        SDL_Delay(10);
    }

    return 0;
}