#include <cstdio>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "constant.h"
#include "player.h"
#include "laser.h"
#include "rocks.h"
#include "bomb.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* backgroundTexture = NULL;

bool running = true;

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

    Player* player = GetPlayer();
    if (keystates[SDL_SCANCODE_W]) player->y -= player->speed;
    if (keystates[SDL_SCANCODE_S]) player->y += player->speed;
    if (keystates[SDL_SCANCODE_A]) player->x -= player->speed;
    if (keystates[SDL_SCANCODE_D]) player->x += player->speed;

    if (player->x < SIZE_DEFAULT / 2) player->x = SIZE_DEFAULT / 2;
    if (player->y < SIZE_DEFAULT / 2) player->y = SIZE_DEFAULT / 2;
    if (player->x > SCREEN_WIDTH - SIZE_DEFAULT / 2) player->x = SCREEN_WIDTH - SIZE_DEFAULT / 2;
    if (player->y > SCREEN_HEIGHT - SIZE_DEFAULT / 2) player->y = SCREEN_HEIGHT - SIZE_DEFAULT / 2;
}

void Render() {
    SDL_RenderClear(gRenderer);

    // Vẽ background
    if (backgroundTexture) {
        SDL_RenderCopy(gRenderer, backgroundTexture, NULL, NULL);
    }

    // Vẽ các đối tượng
    Player_Draw(gRenderer);
    Laser_Draw(gRenderer);
    Rock_Draw(gRenderer);
    Bomb_Draw(gRenderer);

    SDL_RenderPresent(gRenderer);
}

void LoadMedia() {
    backgroundTexture = IMG_LoadTexture(gRenderer, "background.jpg");
    if (!backgroundTexture) {
        printf("Unable to load background texture! SDL_image Error: %s\n", IMG_GetError());
    }

    if (!Rock_LoadTexture(gRenderer)) {
        printf("Unable to load rock texture!\n");
    }

    // Load ảnh player.bmp
    if (!Player_LoadTexture(gRenderer, "player.png")) {
        printf("Unable to load player.bmp!\n");
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    gWindow = SDL_CreateWindow("game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!gWindow) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

    LoadMedia();
    Player_Init();

    Player* player = GetPlayer();
    while (running && player->isAlive) {
        ProcessEvent();
        Laser_Run();
        Rock_Run();
        Bomb_Run();
        Render();
        SDL_Delay(10);
    }

    // Dọn dẹp
    Player_Destroy();

    SDL_DestroyTexture(backgroundTexture);
    Rock_Cleanup();

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
