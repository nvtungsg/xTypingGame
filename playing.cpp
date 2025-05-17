
#include "playing.h"

GamePlay::GamePlay(SDL_Renderer* renderer)
    : renderer(renderer), running(true)
{
    // Khởi tạo biến, load tài nguyên nếu cần
}

GamePlay::~GamePlay() {
    // Giải phóng tài nguyên nếu có
}

void GamePlay::handleEvents(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        running = false;
    }
    // Xử lý thêm sự kiện phím, chuột ở đây nếu cần
}

void GamePlay::update() {
    // Cập nhật trạng thái game (vị trí nhân vật, va chạm, ...)
}

void GamePlay::render() {
    // Vẽ các đối tượng game
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // nền đen
    SDL_RenderClear(renderer);

    // Vẽ nội dung gameplay ở đây

    SDL_RenderPresent(renderer);
}

bool GamePlay::isRunning() const {
    return running;
}
