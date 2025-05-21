#include "sound.h"
#include <iostream>

// Biến toàn cục
Mix_Music* menuMusic = nullptr;
Mix_Chunk* laserSound = nullptr;
Mix_Chunk* bombExplosionSound = nullptr;
Mix_Chunk* explosionSound = nullptr;  // 🔥 Thêm âm thanh nổ tên lửa
Mix_Chunk* bulletSound = nullptr;

bool initSound() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    laserSound = Mix_LoadWAV("laser.mp3");
    if (laserSound == nullptr) {
        std::cout << "Failed to load laser sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    bombExplosionSound = Mix_LoadWAV("bomb_explosion.mp3");
    if (bombExplosionSound == nullptr) {
        std::cout << "Failed to load bomb sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    explosionSound = Mix_LoadWAV("rocket_explosion.mp3");  // 🔥 Load âm thanh tên lửa nổ
    if (explosionSound == nullptr) {
        std::cout << "Failed to load rocket explosion sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    bulletSound = Mix_LoadWAV("bullet.mp3");
    if (bulletSound == nullptr) {
    std::cout << "Failed to load bullet sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
}

    return true;
}

void playMenuMusic() {
    if (menuMusic == nullptr) {
        menuMusic = Mix_LoadMUS("menu_music.mp3");
        if (menuMusic == nullptr) {
            std::cout << "Failed to load menu music! SDL_mixer Error: " << Mix_GetError() << std::endl;
            return;
        }
    }

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(menuMusic, -1);
    }
}

void playLaserSound() {
    if (laserSound != nullptr) {
        Mix_PlayChannel(-1, laserSound, 0);
    }
}

void playBombSound() {
    if (bombExplosionSound != nullptr) {
        Mix_PlayChannel(-1, bombExplosionSound, 0);
    }
}

void playExplosionSound() {
    if (explosionSound != nullptr) {
        Mix_PlayChannel(-1, explosionSound, 0);
    }
}

void playBulletSound() {
    if (bulletSound != nullptr) {
        Mix_PlayChannel(-1, bulletSound, 0);
    }
}

void stopMenuMusic() {
    Mix_HaltMusic();
    if (menuMusic != nullptr) {
        Mix_FreeMusic(menuMusic);
        menuMusic = nullptr;
    }
}

void cleanUpSound() {
    stopMenuMusic();

    if (laserSound != nullptr) {
        Mix_FreeChunk(laserSound);
        laserSound = nullptr;
    }

    if (bombExplosionSound != nullptr) {
        Mix_FreeChunk(bombExplosionSound);
        bombExplosionSound = nullptr;
    }

    if (explosionSound != nullptr) {
        Mix_FreeChunk(explosionSound);
        explosionSound = nullptr;
    }

    Mix_CloseAudio();
}

// Thêm hàm điều chỉnh âm lượng nhạc nền menu
void setMenuMusicVolume(int volume) {
    Mix_VolumeMusic(volume);  // volume từ 0 đến 128
}
