#include "menu.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cstdio>

static SDL_Renderer* gRenderer = nullptr;
static TTF_Font* gFontNormal = nullptr;
static TTF_Font* gFontBold = nullptr;

enum MenuScreen {
    SCREEN_MENU,
    SCREEN_OPTIONS
};

static MenuScreen currentScreen = SCREEN_MENU;

// Vị trí chữ menu
static SDL_Rect playRect = {540, 250, 200, 60};
static SDL_Rect optionsRect = {540, 330, 200, 60};
static SDL_Rect exitRect = {540, 410, 200, 60};

// Ảnh màn hình Options
static SDL_Texture* optionsImage = nullptr;
static SDL_Rect escRect = {20, 20, 80, 40};
static SDL_Texture* escTexture = nullptr;

// Background menu
static SDL_Texture* menuBackground = nullptr;

// Độ phóng to khi hover
const float HOVER_SCALE = 1.2f;

// Trạng thái hover
static bool hoverPlay = false;
static bool hoverOptions = false;
static bool hoverExit = false;

static SDL_Color normalColor = {255, 255, 255, 255};    // trắng
static SDL_Color hoverColor = {255, 255, 255, 255};     // trắng (hover vẫn trắng nhưng in đậm)

// Tạo texture chữ với font truyền vào
static SDL_Texture* CreateTextTexture(const char* text, SDL_Color color, SDL_Rect& dstRect, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) {
        printf("Failed to create surface from text: %s\n", TTF_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    dstRect.w = surface->w;
    dstRect.h = surface->h;
    SDL_FreeSurface(surface);
    return texture;
}

static bool IsPointInRect(int x, int y, const SDL_Rect& rect) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

bool Menu_LoadBackground(const char* filepath) {
    if (menuBackground) {
        SDL_DestroyTexture(menuBackground);
        menuBackground = nullptr;
    }
    menuBackground = IMG_LoadTexture(gRenderer, filepath);
    if (!menuBackground) {
        printf("Failed to load background image %s: %s\n", filepath, SDL_GetError());
        return false;
    }
    return true;
}

void Menu_Init(SDL_Renderer* renderer) {
    gRenderer = renderer;

    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return;
    }

    gFontNormal = TTF_OpenFont("arial.ttf", 24);
    if (!gFontNormal) {
        printf("Failed to load normal font: %s\n", TTF_GetError());
        return;
    }
    gFontBold = TTF_OpenFont("arial.ttf", 24);
    if (!gFontBold) {
        printf("Failed to load bold font: %s\n", TTF_GetError());
        return;
    }
    TTF_SetFontStyle(gFontBold, TTF_STYLE_BOLD);

    optionsImage = IMG_LoadTexture(gRenderer, "controls.png");
    escTexture = CreateTextTexture("ESC", normalColor, escRect, gFontNormal);

    // Load background menu mặc định
    Menu_LoadBackground("menu_bg.png");
}

void Menu_Cleanup() {
    if (gFontNormal) {
        TTF_CloseFont(gFontNormal);
        gFontNormal = nullptr;
    }
    if (gFontBold) {
        TTF_CloseFont(gFontBold);
        gFontBold = nullptr;
    }
    TTF_Quit();

    if (optionsImage) {
        SDL_DestroyTexture(optionsImage);
        optionsImage = nullptr;
    }
    if (escTexture) {
        SDL_DestroyTexture(escTexture);
        escTexture = nullptr;
    }
    if (menuBackground) {
        SDL_DestroyTexture(menuBackground);
        menuBackground = nullptr;
    }
}

MenuResult Menu_HandleEvent(SDL_Event* e) {
    int mx, my;

    if (currentScreen == SCREEN_MENU) {
        if (e->type == SDL_MOUSEMOTION) {
            mx = e->motion.x;
            my = e->motion.y;
            hoverPlay = IsPointInRect(mx, my, playRect);
            hoverOptions = IsPointInRect(mx, my, optionsRect);
            hoverExit = IsPointInRect(mx, my, exitRect);
        }
        else if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            mx = e->button.x;
            my = e->button.y;
            if (IsPointInRect(mx, my, playRect)) {
                return MENU_PLAY;
            }
            else if (IsPointInRect(mx, my, optionsRect)) {
                currentScreen = SCREEN_OPTIONS;
            }
            else if (IsPointInRect(mx, my, exitRect)) {
                return MENU_EXIT;
            }
        }
    }
    else if (currentScreen == SCREEN_OPTIONS) {
        if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
            mx = e->button.x;
            my = e->button.y;
            if (IsPointInRect(mx, my, escRect)) {
                currentScreen = SCREEN_MENU;
            }
        }
        else if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                currentScreen = SCREEN_MENU;
            }
        }
    }

    if (e->type == SDL_QUIT) {
        return MENU_EXIT;
    }

    return MENU_NONE;
}

void Menu_Render() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    if (currentScreen == SCREEN_MENU) {
        // Vẽ background menu nếu có
        if (menuBackground) {
            SDL_RenderCopy(gRenderer, menuBackground, nullptr, nullptr);
        }

        // Vẽ chữ menu với hiệu ứng hover (phóng to và in đậm màu trắng)
        SDL_Texture* tex;
        SDL_Rect rect;

        // Play
        tex = CreateTextTexture("Play", normalColor, rect, hoverPlay ? gFontBold : gFontNormal);
        rect.x = playRect.x + (playRect.w - rect.w) / 2;
        rect.y = playRect.y + (playRect.h - rect.h) / 2;
        if (hoverPlay) {
            int dw = int(rect.w * (HOVER_SCALE - 1) / 2);
            int dh = int(rect.h * (HOVER_SCALE - 1) / 2);
            rect.x -= dw;
            rect.y -= dh;
            rect.w = int(rect.w * HOVER_SCALE);
            rect.h = int(rect.h * HOVER_SCALE);
        }
        SDL_RenderCopy(gRenderer, tex, nullptr, &rect);
        SDL_DestroyTexture(tex);

        // Options
        tex = CreateTextTexture("Options", normalColor, rect, hoverOptions ? gFontBold : gFontNormal);
        rect.x = optionsRect.x + (optionsRect.w - rect.w) / 2;
        rect.y = optionsRect.y + (optionsRect.h - rect.h) / 2;
        if (hoverOptions) {
            int dw = int(rect.w * (HOVER_SCALE - 1) / 2);
            int dh = int(rect.h * (HOVER_SCALE - 1) / 2);
            rect.x -= dw;
            rect.y -= dh;
            rect.w = int(rect.w * HOVER_SCALE);
            rect.h = int(rect.h * HOVER_SCALE);
        }
        SDL_RenderCopy(gRenderer, tex, nullptr, &rect);
        SDL_DestroyTexture(tex);

        // Exit
        tex = CreateTextTexture("Exit", normalColor, rect, hoverExit ? gFontBold : gFontNormal);
        rect.x = exitRect.x + (exitRect.w - rect.w) / 2;
        rect.y = exitRect.y + (exitRect.h - rect.h) / 2;
        if (hoverExit) {
            int dw = int(rect.w * (HOVER_SCALE - 1) / 2);
            int dh = int(rect.h * (HOVER_SCALE - 1) / 2);
            rect.x -= dw;
            rect.y -= dh;
            rect.w = int(rect.w * HOVER_SCALE);
            rect.h = int(rect.h * HOVER_SCALE);
        }
        SDL_RenderCopy(gRenderer, tex, nullptr, &rect);
        SDL_DestroyTexture(tex);
    }
    else if (currentScreen == SCREEN_OPTIONS) {
        if (optionsImage) {
            SDL_RenderCopy(gRenderer, optionsImage, nullptr, nullptr);
        }
        if (escTexture) {
            SDL_RenderCopy(gRenderer, escTexture, nullptr, &escRect);
        }
    }

    SDL_RenderPresent(gRenderer);
}
