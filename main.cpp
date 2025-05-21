#include <cstdio>
#include <SDL_mixer.h>
#include "bullet.h"
#include <SDL_ttf.h>
#include "sound.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "gameover.h"
#include "constant.h"
#include "player.h"
#include "laser.h"
#include "rocks.h"
#include "bomb.h"
#include "menu.h"
#include "score.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* backgroundTexture = NULL;
SDL_Texture* continueTextTexture = NULL;
SDL_Texture* menuTextTexture = NULL;
SDL_Rect continueTextRect;
SDL_Rect menuTextRect;
bool running = true;
bool soundInitialized = false;

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSE,
    STATE_GAMEOVER_MENU  // Menu giống Pause nhưng dành cho Game Over
};

GameState gameState = STATE_MENU;
bool hoverContinue = false;
bool hoverMenu = false;

// Thời gian đếm để hiện menu game over sau 3 giây
Uint32 gameOverStartTime = 0;
bool gameOverMenuReady = false;


void RenderPauseOrGameOverMenu() {
    // Overlay làm mờ nền
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(gRenderer, &overlay);

    if (gameState == STATE_GAMEOVER_MENU && !gameOverMenuReady) {
        // Hiện chữ GAME OVER to, đậm giữa màn hình
        TTF_Font* fontBig = TTF_OpenFont("arial.ttf", 72);
        if (fontBig) {
            SDL_Color red = {255, 0, 0, 255};
            SDL_Surface* surfGameOver = TTF_RenderText_Blended(fontBig, "GAME OVER", red);
            if (surfGameOver) {
                SDL_Texture* texGameOver = SDL_CreateTextureFromSurface(gRenderer, surfGameOver);
                SDL_Rect rectGameOver = {
                    SCREEN_WIDTH / 2 - surfGameOver->w / 2,
                    SCREEN_HEIGHT / 2 - surfGameOver->h / 2,
                    surfGameOver->w,
                    surfGameOver->h
                };
                SDL_RenderCopy(gRenderer, texGameOver, NULL, &rectGameOver);
                SDL_DestroyTexture(texGameOver);
                SDL_FreeSurface(surfGameOver);
            }
            TTF_CloseFont(fontBig);
        }
        return; // Chưa đến 3 giây, chưa hiển thị nút
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePos = { mx, my };

    hoverContinue = SDL_PointInRect(&mousePos, &continueTextRect);
    hoverMenu = SDL_PointInRect(&mousePos, &menuTextRect);

    SDL_Color normalColor = {255, 255, 255, 255};
    SDL_Color hoverColor = {255, 215, 0, 255};

    SDL_SetTextureColorMod(continueTextTexture,
        hoverContinue ? hoverColor.r : normalColor.r,
        hoverContinue ? hoverColor.g : normalColor.g,
        hoverContinue ? hoverColor.b : normalColor.b);
    SDL_RenderCopy(gRenderer, continueTextTexture, NULL, &continueTextRect);

    SDL_Rect borderRect = continueTextRect;
    borderRect.x -= 5; borderRect.y -= 5;
    borderRect.w += 10; borderRect.h += 10;
    SDL_SetRenderDrawColor(gRenderer,
        hoverContinue ? hoverColor.r : normalColor.r,
        hoverContinue ? hoverColor.g : normalColor.g,
        hoverContinue ? hoverColor.b : normalColor.b, 255);
    SDL_RenderDrawRect(gRenderer, &borderRect);

    SDL_SetTextureColorMod(menuTextTexture,
        hoverMenu ? hoverColor.r : normalColor.r,
        hoverMenu ? hoverColor.g : normalColor.g,
        hoverMenu ? hoverColor.b : normalColor.b);
    SDL_RenderCopy(gRenderer, menuTextTexture, NULL, &menuTextRect);

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
    if (backgroundTexture) {
        SDL_RenderCopy(gRenderer, backgroundTexture, NULL, NULL);
    }
    Player_Draw(gRenderer);
    Bullet_Draw(gRenderer);
    Laser_Draw(gRenderer);
    Rock_Draw(gRenderer);
    Bomb_Draw(gRenderer);
    Score_Render();

    if (gameState == STATE_PAUSE || gameState == STATE_GAMEOVER_MENU) {
        RenderPauseOrGameOverMenu();
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

bool LoadPauseMenuText() {
    TTF_Font* font = TTF_OpenFont("arial.ttf", 36);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return false;
    }

    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* surfContinue = TTF_RenderText_Blended(font, "Continue", white);
    if (!surfContinue) {
        printf("Failed to create Play Again surface: %s\n", TTF_GetError());
        TTF_CloseFont(font);
        return false;
    }

    continueTextTexture = SDL_CreateTextureFromSurface(gRenderer, surfContinue);
    continueTextRect = {
        SCREEN_WIDTH / 2 - surfContinue->w / 2,
        SCREEN_HEIGHT / 2 - 60,
        surfContinue->w,
        surfContinue->h
    };
    SDL_FreeSurface(surfContinue);

    SDL_Surface* surfMenu = TTF_RenderText_Blended(font, "Back to Menu", white);
    if (!surfMenu) {
        SDL_DestroyTexture(continueTextTexture);
        TTF_CloseFont(font);
        return false;
    }

    menuTextTexture = SDL_CreateTextureFromSurface(gRenderer, surfMenu);
    menuTextRect = {
        SCREEN_WIDTH / 2 - surfMenu->w / 2,
        SCREEN_HEIGHT / 2 + 20,
        surfMenu->w,
        surfMenu->h
    };
    SDL_FreeSurface(surfMenu);
    TTF_CloseFont(font);
    return true;
}


void ChangeGameState(GameState newState) {
    if (gameState != newState) {
        gameState = newState;

        if (gameState == STATE_MENU) {
            // Nhạc nền menu âm lượng max
            setMenuMusicVolume(128);
        } else if (gameState == STATE_PLAYING) {
            // Nhạc nền menu âm lượng nhỏ khi chơi game
            setMenuMusicVolume(40);
        }
    }
}


TTF_Font* gFont = nullptr;


int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { // <-- Thêm SDL_INIT_AUDIO
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!initSound()) { // <-- NEW
        printf("Failed to initialize sound.\n");
        SDL_Quit();
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
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    LoadMedia();
    Bomb_LoadTextures(gRenderer);
    Menu_Init(gRenderer);
    Score_Init(gRenderer);

    if (!LoadPauseMenuText()) {
        printf("Failed to load pause menu text.\n");
        running = false;
    }

    Player* player = GetPlayer();

    playMenuMusic(); // <-- NEW: Bắt đầu nhạc khi khởi động game (Menu)

    while (running) {
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }

            if (gameState == STATE_MENU) {
                MenuResult res = Menu_HandleEvent(&e);
                if (res == MENU_PLAY) {
                    ChangeGameState(STATE_PLAYING);
                    Rock_Reset();
                    Bomb_Reset();
                    Bullet_Reset();
                    Laser_Reset();
                    gameOverStartTime = 0;
                    gameOverMenuReady = false;
                    gameState = STATE_PLAYING;
                    Player_Init();
                    Score_Reset();
                } else if (res == MENU_EXIT) {
                    running = false;
                }

            } else if (gameState == STATE_PLAYING) {
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                    gameState = STATE_PAUSE;
                }

            } else if (gameState == STATE_PAUSE || gameState == STATE_GAMEOVER_MENU) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;
                    SDL_Point mousePoint = { mx, my };

                    if (SDL_PointInRect(&mousePoint, &continueTextRect)) {
                        if (gameState == STATE_GAMEOVER_MENU) {
                            Player_Init();
                            Score_Reset();
                            Rock_Reset();
                            Bullet_Reset();
                            Bomb_Reset();
                            Laser_Reset();
                            gameOverStartTime = 0;
                            gameOverMenuReady = false;
                        }
                        gameState = STATE_PLAYING;
                    } else if (SDL_PointInRect(&mousePoint, &menuTextRect)) {
                        gameState = STATE_MENU;
                        gameOverStartTime = 0;
                        gameOverMenuReady = false;

                        playMenuMusic(); // <-- NEW: Phát lại nhạc khi quay về menu
                    }
                }
            }
        }

        if (gameState == STATE_MENU) {
            Menu_Render();

        } else if (gameState == STATE_PLAYING || gameState == STATE_PAUSE || gameState == STATE_GAMEOVER_MENU) {
            if (player->isAlive) {
                if (gameState == STATE_PLAYING) {
                    ProcessEvent();
                    Bullet_Run();
                    Laser_Run();
                    Rock_Run();
                    Bomb_Run();
                    Score_Update(10);
                }
                Render();
            } else {
                if (gameOverStartTime == 0) {
                    gameOverStartTime = SDL_GetTicks();
                    gameOverMenuReady = false;
                } else {
                    Uint32 now = SDL_GetTicks();
                    if (now - gameOverStartTime >=1500) {
                        gameOverMenuReady = true;
                    }
                }

                gameState = STATE_GAMEOVER_MENU;
                Render();
            }
        }

        SDL_Delay(10);
    }

    Player_Destroy();
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(continueTextTexture);
    SDL_DestroyTexture(menuTextTexture);
    Rock_Cleanup();
    Menu_Cleanup();
    Score_Cleanup();
    playMenuMusic();  // phát nhạc menu, loop vô hạn
    setMenuMusicVolume(128); // âm lượng lớn khi mới vào menu


    cleanUpSound(); // <-- NEW: Giải phóng âm thanh

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
