#include <cstdio>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "constant.h"
#include "player.h"
#include "laser.h"
#include "rocks.h"
#include "bomb.h"
#include "menu.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* backgroundTexture = NULL;

// Text textures cho pause menu
SDL_Texture* continueTextTexture = NULL;
SDL_Texture* menuTextTexture = NULL;
SDL_Rect continueTextRect;
SDL_Rect menuTextRect;

bool running = true;

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSE,
    STATE_GAMEOVER
};

GameState gameState = STATE_MENU;

// Hover trạng thái
bool hoverContinue = false;
bool hoverMenu = false;

void RenderPauseMenu() {
    // Vẽ overlay đen mờ
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(gRenderer, &overlay);

    // Lấy vị trí chuột
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePos = { mx, my };

    // Kiểm tra hover
    hoverContinue = SDL_PointInRect(&mousePos, &continueTextRect);
    hoverMenu = SDL_PointInRect(&mousePos, &menuTextRect);

    SDL_Color normalColor = {255, 255, 255, 255};    // Trắng
    SDL_Color hoverColor = {255, 215, 0, 255};       // Vàng

    // Vẽ chữ Continue với màu hover nếu có
    SDL_SetTextureColorMod(continueTextTexture,
        hoverContinue ? hoverColor.r : normalColor.r,
        hoverContinue ? hoverColor.g : normalColor.g,
        hoverContinue ? hoverColor.b : normalColor.b);
    SDL_RenderCopy(gRenderer, continueTextTexture, NULL, &continueTextRect);

    // Vẽ khung viền quanh chữ Continue
    SDL_Rect borderRect = continueTextRect;
    borderRect.x -= 5; borderRect.y -= 5;
    borderRect.w += 10; borderRect.h += 10;
    SDL_SetRenderDrawColor(gRenderer,
        hoverContinue ? hoverColor.r : normalColor.r,
        hoverContinue ? hoverColor.g : normalColor.g,
        hoverContinue ? hoverColor.b : normalColor.b, 255);
    SDL_RenderDrawRect(gRenderer, &borderRect);

    // Vẽ chữ Back to Menu
    SDL_SetTextureColorMod(menuTextTexture,
        hoverMenu ? hoverColor.r : normalColor.r,
        hoverMenu ? hoverColor.g : normalColor.g,
        hoverMenu ? hoverColor.b : normalColor.b);
    SDL_RenderCopy(gRenderer, menuTextTexture, NULL, &menuTextRect);

    // Vẽ khung viền quanh chữ Back to Menu
    borderRect = menuTextRect;
    borderRect.x -= 5; borderRect.y -= 5;
    borderRect.w += 10; borderRect.h += 10;
    SDL_SetRenderDrawColor(gRenderer,
        hoverMenu ? hoverColor.r : normalColor.r,
        hoverMenu ? hoverColor.g : normalColor.g,
        hoverMenu ? hoverColor.b : normalColor.b, 255);
    SDL_RenderDrawRect(gRenderer, &borderRect);
}

void ProcessEvent() {
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

    Player_Draw(gRenderer);
    Laser_Draw(gRenderer);
    Rock_Draw(gRenderer);
    Bomb_Draw(gRenderer);

    if (gameState == STATE_PAUSE) {
        RenderPauseMenu();
    }

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

    if (!Player_LoadTexture(gRenderer, "player.png")) {
        printf("Unable to load player.png!\n");
    }
}

// Hàm load font và tạo text texture pause menu
bool LoadPauseMenuText() {
    TTF_Font* font = TTF_OpenFont("arial.ttf", 36);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return false;
    }

    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* surfContinue = TTF_RenderText_Blended(font, "Continue", white);
    if (!surfContinue) {
        printf("Failed to create Continue surface: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return false;
    }
    continueTextTexture = SDL_CreateTextureFromSurface(gRenderer, surfContinue);
    continueTextRect.w = surfContinue->w;
    continueTextRect.h = surfContinue->h;
    continueTextRect.x = SCREEN_WIDTH / 2 - continueTextRect.w / 2;
    continueTextRect.y = SCREEN_HEIGHT / 2 - 60;
    SDL_FreeSurface(surfContinue);

    SDL_Surface* surfMenu = TTF_RenderText_Blended(font, "Back to Menu", white);
    if (!surfMenu) {
        printf("Failed to create Back to Menu surface: %s\n", TTF_GetError());
        SDL_DestroyTexture(continueTextTexture);
        continueTextTexture = NULL;
        TTF_CloseFont(font);
        return false;
    }
    menuTextTexture = SDL_CreateTextureFromSurface(gRenderer, surfMenu);
    menuTextRect.w = surfMenu->w;
    menuTextRect.h = surfMenu->h;
    menuTextRect.x = SCREEN_WIDTH / 2 - menuTextRect.w / 2;
    menuTextRect.y = SCREEN_HEIGHT / 2 + 20;
    SDL_FreeSurface(surfMenu);

    TTF_CloseFont(font);
    return true;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    gWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!gWindow) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

    LoadMedia();
    Bomb_LoadTextures(gRenderer);
    Menu_Init(gRenderer);

    if (!LoadPauseMenuText()) {
        printf("Failed to load pause menu text.\n");
        running = false;
    }

    Player* player = GetPlayer();

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (gameState == STATE_MENU) {
                MenuResult res = Menu_HandleEvent(&e);
                if (res == MENU_PLAY) {
                    gameState = STATE_PLAYING;
                    Player_Init();
                } else if (res == MENU_EXIT) {
                    running = false;
                }

            } else if (gameState == STATE_PLAYING) {
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                    gameState = STATE_PAUSE;
                }

            } else if (gameState == STATE_PAUSE) {
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_RETURN) {
                        gameState = STATE_PLAYING;
                    } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                        gameState = STATE_MENU;
                    }
                } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;
                    SDL_Point mousePoint = { mx, my };
                    if (SDL_PointInRect(&mousePoint, &continueTextRect)) {
                        gameState = STATE_PLAYING;
                    } else if (SDL_PointInRect(&mousePoint, &menuTextRect)) {
                        gameState = STATE_MENU;
                    }
                }
            }
        }

        if (gameState == STATE_MENU) {
            Menu_Render();
        } else if (gameState == STATE_PLAYING || gameState == STATE_PAUSE) {
            if (player->isAlive) {
                if (gameState == STATE_PLAYING) {
                    ProcessEvent();
                    Laser_Run();
                    Rock_Run();
                    Bomb_Run();
                }
                Render();
            } else {
                gameState = STATE_GAMEOVER;
                running = false;
            }
        }

        SDL_Delay(10);
    }

    Player_Destroy();

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(bombImage);
    SDL_DestroyTexture(explosionImage);
    SDL_DestroyTexture(continueTextTexture);
    SDL_DestroyTexture(menuTextTexture);
    Rock_Cleanup();
    Menu_Cleanup();

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
