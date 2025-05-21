#ifndef MENU_H
#define MENU_H

#include <SDL.h>

enum MenuResult {
    MENU_NONE,
    MENU_PLAY,
    MENU_EXIT
};

void Menu_Init(SDL_Renderer* renderer);
MenuResult Menu_HandleEvent(SDL_Event* e);
void Menu_Render();          // Không nhận tham số, vì gRenderer là biến static trong menu.cpp
void Menu_Cleanup();         // Thêm khai báo hàm này
#endif
