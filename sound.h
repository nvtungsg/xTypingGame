#ifndef SOUND_H
#define SOUND_H

#include "SDL_mixer.h"

extern Mix_Music* menuMusic;
extern Mix_Chunk* laserSound;
extern Mix_Chunk* bombExplosionSound;
extern Mix_Chunk* bulletSound;

bool initSound();
void playMenuMusic();
void stopMenuMusic();
void playLaserSound();
void playBombSound();
void playBulletSound();
void cleanUpSound();
void playExplosionSound();  // Cho hiệu ứng tên lửa nổ
void setMenuMusicVolume(int volume);


#endif
