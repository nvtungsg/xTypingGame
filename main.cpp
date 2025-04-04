#include <iostream>
#include <SDL.h>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL không thể khởi tạo! Lỗi: " << SDL_GetError() << endl;
        return 1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Pong Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Không thể tạo cửa sổ! Lỗi: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Không thể tạo renderer! Lỗi: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Tải ảnh BMP
    SDL_Surface* imageSurface = SDL_LoadBMP("intro.bmp");
    if (!imageSurface) {
        cout << "Không thể tải ảnh! Lỗi: " << SDL_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Chuyển ảnh thành texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);
    if (!texture) {
        cout << "Không thể tạo texture! Lỗi: " << SDL_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Xác định vị trí các nút trong ảnh (x, y, width, height)
    SDL_Rect playButton = {300, 250, 200, 50};  // Nút PLAY
    SDL_Rect tutorialButton = {300, 320, 200, 50};  // Nút TUTORIAL
    SDL_Rect exitButton = {300, 390, 200, 50};  // Nút EXIT

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }

            // Kiểm tra click chuột
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Kiểm tra nếu bấm vào nút PLAY
                if (mouseX >= playButton.x && mouseX <= playButton.x + playButton.w &&
                    mouseY >= playButton.y && mouseY <= playButton.y + playButton.h) {
                    cout << "Bấm vào nút PLAY" << endl;
                    // TODO: Chuyển sang màn hình chơi game
                }

                // Kiểm tra nếu bấm vào nút TUTORIAL
                if (mouseX >= tutorialButton.x && mouseX <= tutorialButton.x + tutorialButton.w &&
                    mouseY >= tutorialButton.y && mouseY <= tutorialButton.y + tutorialButton.h) {
                    cout << "Bấm vào nút TUTORIAL" << endl;
                    // TODO: Hiển thị hướng dẫn chơi
                }

                // Kiểm tra nếu bấm vào nút EXIT
                if (mouseX >= exitButton.x && mouseX <= exitButton.x + exitButton.w &&
                    mouseY >= exitButton.y && mouseY <= exitButton.y + exitButton.h) {
                    cout << "Bấm vào nút EXIT, thoát game!" << endl;
                    running = false;
                }
            }
        }

        // Xóa màn hình
        SDL_RenderClear(renderer);

        // Hiển thị ảnh nền với kích thước cửa sổ
        SDL_Rect destRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        // Cập nhật màn hình
        SDL_RenderPresent(renderer);
    }

    // Dọn dẹp bộ nhớ
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
