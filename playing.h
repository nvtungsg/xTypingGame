#ifndef PLAYING_H
#define PLAYING_H

#include <SDL.h>

class GamePlay {
public:
    GamePlay(SDL_Renderer* renderer);
    ~GamePlay();

    void handleEvents(SDL_Event& e);
    void update();
    void render();

    bool isRunning() const;

private:
    SDL_Renderer* renderer;
    bool running;
};

#endif // PLAYING_H

