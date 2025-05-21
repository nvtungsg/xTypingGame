#include "gameover.h"
#include <SDL.h>
#include <SDL_ttf.h>

// Biến trạng thái
static int gameOverStartTime = 0;
static bool showButtons = false;

// Nút (tạm thời dùng hình chữ nhật, sau có thể dùng ảnh)
SDL_Rect playAgainRect = { 490, 400, 300, 60 };
SDL_Rect backToMenuRect = { 490, 480, 300, 60 };

// Font và renderer để vẽ chữ (nếu muốn sau này)
extern TTF_Font* gFont;

// Hàm khởi tạo khi Game Over bắt đầu
void GameOver_Start() {
    gameOverStartTime = SDL_GetTicks();
    showButtons = false;
}

// Cập nhật thời gian để hiển thị nút sau 3 giây
void GameOver_Update() {
    if (SDL_GetTicks() - gameOverStartTime >= 3000) {
        showButtons = true;
    }
}

// Vẽ màn hình Game Over
void GameOver_Render(SDL_Renderer* renderer) {
    // Làm mờ màn hình
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180); // đen mờ
    SDL_Rect overlay = { 0, 0, 1280, 720 };
    SDL_RenderFillRect(renderer, &overlay);

    // Vẽ chữ "GAME OVER"
    if (gFont) {
        SDL_Color textColor = { 255, 0, 0 };
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(gFont, "GAME OVER", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = { 390, 200, textSurface->w, textSurface->h };
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
    }

    // Nếu đã đủ 3 giây thì hiển thị nút
    if (showButtons) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // trắng

        // Vẽ nút Play Again
        SDL_RenderFillRect(renderer, &playAgainRect);

        // Vẽ nút Back to Menu
        SDL_RenderFillRect(renderer, &backToMenuRect);

        // Vẽ chữ trên nút
        if (gFont) {
            SDL_Color black = { 0, 0, 0 };
            SDL_Surface* text1 = TTF_RenderUTF8_Blended(gFont, "Play Again", black);
            SDL_Surface* text2 = TTF_RenderUTF8_Blended(gFont, "Back to Menu", black);
            SDL_Texture* tex1 = SDL_CreateTextureFromSurface(renderer, text1);
            SDL_Texture* tex2 = SDL_CreateTextureFromSurface(renderer, text2);

            SDL_Rect rect1 = { playAgainRect.x + 50, playAgainRect.y + 15, text1->w, text1->h };
            SDL_Rect rect2 = { backToMenuRect.x + 35, backToMenuRect.y + 15, text2->w, text2->h };

            SDL_RenderCopy(renderer, tex1, NULL, &rect1);
            SDL_RenderCopy(renderer, tex2, NULL, &rect2);

            SDL_FreeSurface(text1);
            SDL_FreeSurface(text2);
            SDL_DestroyTexture(tex1);
            SDL_DestroyTexture(tex2);
        }
    }
}

// Kiểm tra nếu click vào nút Play Again
bool GameOver_IsPlayAgainClicked(SDL_Point p) {
    return showButtons && SDL_PointInRect(&p, &playAgainRect);
}

// Kiểm tra nếu click vào nút Back to Menu
bool GameOver_IsBackToMenuClicked(SDL_Point p) {
    return showButtons && SDL_PointInRect(&p, &backToMenuRect);
}
