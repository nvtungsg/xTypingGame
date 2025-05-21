#include "score.h"
#include <SDL_ttf.h>
#include <cstdio>
#include <string>

static SDL_Renderer* gRenderer = nullptr;
static TTF_Font* gFont = nullptr;
static int currentScore = 0;
static int recordScore = 0;
int SCORES = 0;
static SDL_Texture* scoreTexture = nullptr;
static SDL_Texture* recordTexture = nullptr;
static SDL_Rect scoreRect = {20, 20, 0, 0};
static SDL_Rect recordRect = {20, 60, 0, 0};

static float timeAccumulator = 0.0f;

static SDL_Color whiteColor = {255, 255, 255, 255};

static SDL_Texture* CreateTextTexture(const char* text, SDL_Color color, SDL_Rect& dstRect) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(gFont, text, color);
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

void Score_Init(SDL_Renderer* renderer) {
    gRenderer = renderer;

    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return;
    }

    gFont = TTF_OpenFont("arial.ttf", 24);
    if (!gFont) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    currentScore = 0;
    recordScore = 0; // Bạn có thể load record từ file ở đây nếu muốn
}

void Score_Reset() {
    currentScore = 0;
    timeAccumulator = 0.0f;
}

void Score_AddTime(float deltaTime) {
    timeAccumulator += deltaTime;
    if (timeAccumulator >= 1.0f) {
        currentScore++;
        SCORES = currentScore;
        timeAccumulator = 0.0f;
    }
}

int Score_GetCurrent() {
    return currentScore;
}

int Score_GetRecord() {
    return recordScore;
}

void Score_Update(float deltaTime) {
    Score_AddTime(deltaTime);

    // Cập nhật recordScore nếu currentScore vượt qua
    if (currentScore > recordScore) {
        recordScore = currentScore;
    }
}


void Score_Render() {
    // Xoá texture cũ trước
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }
    if (recordTexture) {
        SDL_DestroyTexture(recordTexture);
        recordTexture = nullptr;
    }

    // Tạo text mới
    std::string scoreText = "SCORES: " + std::to_string(currentScore);
    std::string recordText = "RECORD: " + std::to_string(recordScore);

    scoreTexture = CreateTextTexture(scoreText.c_str(), whiteColor, scoreRect);
    recordTexture = CreateTextTexture(recordText.c_str(), whiteColor, recordRect);

    // Vẽ lên màn hình
    if (scoreTexture) {
        SDL_RenderCopy(gRenderer, scoreTexture, nullptr, &scoreRect);
    }
    if (recordTexture) {
        SDL_RenderCopy(gRenderer, recordTexture, nullptr, &recordRect);
    }
}

void Score_Cleanup() {
    if (scoreTexture) {
        SDL_DestroyTexture(scoreTexture);
        scoreTexture = nullptr;
    }
    if (recordTexture) {
        SDL_DestroyTexture(recordTexture);
        recordTexture = nullptr;
    }
    if (gFont) {
        TTF_CloseFont(gFont);
        gFont = nullptr;
    }
    TTF_Quit();
}

void Score_SetRecord(int newRecord) {
    recordScore = newRecord;
}
