#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#if defined(_WIN32) // Use local headers for Windows
#include "../AL/al.h"
#include "../AL/alc.h"
#include "../sndfile/sndfile.h"
#else // Use system headers for linux
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>
#endif

#include <fstream>
#include <vector>

typedef struct {
  ALuint buffer;
  ALuint source;
} Sound;

enum SoundList {
  ATypeTileBreak,
  ATypeSand,
  ATypeGrass,
};

extern std::vector<Sound> sounds;

void InitOpenAL();
void CloseOpenAL();

bool loadSound(const std::string& filename, Sound& sound);

void playSound(int index);
void stopSound(int index);

void loadSounds();
void unloadSounds();


#endif