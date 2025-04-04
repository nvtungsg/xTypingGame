#include <iostream>
#include <SDL.h>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

enum MenuOption { PLAY, TUTORIAL, EXIT }; // Các lựa chọn trong menu

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Pong Game Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Tải ảnh menu
    SDL_Surface* surface = SDL_LoadBMP("menu.bmp");
    if (!surface) {
        cout << "Không tải được ảnh menu: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    bool running = true;
    SDL_Event event;
    MenuOption selectedOption = PLAY; // Mặc định chọn "PLAY"

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_DOWN) {
                    if (selectedOption == PLAY) selectedOption = TUTORIAL;
                    else if (selectedOption == TUTORIAL) selectedOption = EXIT;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    if (selectedOption == EXIT) selectedOption = TUTORIAL;
                    else if (selectedOption == TUTORIAL) selectedOption = PLAY;
                }
                if (event.key.keysym.sym == SDLK_RETURN) { // Nhấn Enter để chọn
                    if (selectedOption == PLAY) {
                        cout << "Chọn PLAY" << endl;
                        running = false; // Thoát menu để vào game
                    } else if (selectedOption == TUTORIAL) {
                        cout << "Chọn TUTORIAL" << endl;
                    } else if (selectedOption == EXIT) {
                        running = false; // Thoát game
                    }
                }
            }
        }

        // Vẽ background
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
