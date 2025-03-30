#include "../include/Sandboxia/audio.h"
#include "../include/main.h"
#include "../include/Sandboxia/input.h"
#include "../include/Sandboxia/player.h"

ALCdevice* device;
ALCcontext* context;

std::vector<Sound> sounds;

void InitOpenAL() {
  device = alcOpenDevice(NULL); // Use the default device

  // Make sure that initialization was successful

  if (!device) {
    log("[ERROR] Failed to open OpenAL device");
    return;
  }

  context = alcCreateContext(device, NULL);

  if (!context) {
    log("[ERROR] Failed to create OpenAL context");
    alcCloseDevice(device);
    return;
  }

  alcMakeContextCurrent(context);

  log("[INFO] OpenAL initialized");
}

void CloseOpenAL() {
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);

  log("[INFO] OpenAL closed");
}

bool loadSound(const std::string& filename, Sound& sound) {
  SF_INFO sfInfo; // Open sound file
  SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &sfInfo);
  if (!file) {
    log("[ERROR] Failed to open sound file: " + filename);
    return false;
  }

  // Allocate memory for audio data
  std::vector<short> samples(sfInfo.frames * sfInfo.channels);
  sf_read_short(file, samples.data(), samples.size());
  sf_close(file);

  // Determine OpenAL format
  ALenum format = (sfInfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

  // Generate and load buffer
  alGenBuffers(1, &sound.buffer);
  alBufferData(sound.buffer, format, samples.data(), samples.size() * sizeof(short), sfInfo.samplerate);

  // Generate source
  alGenSources(1, &sound.source);
  alSourcei(sound.source, AL_BUFFER, sound.buffer);

  return true;
}

void playSound(int index) {
  if (!SOUNDS_ENABLED) return;
  if (index < 0 || index >= sounds.size()) { // Prevent crash
    log("[ERROR] Invalid sound index: " + std::to_string(index));
    return;
  }
  alSourceStop(sounds[index].source);
  alSourcePlay(sounds[index].source);
}

void stopSound(int index) {
  if (index < 0 || index >= sounds.size()) { // Prevent crash
    log("[ERROR] Invalid sound index: " + std::to_string(index));
    return;
  }
  ALuint source = sounds[index].source;
  alSourceStop(source);
  alDeleteSources(1, &source);
}

void loadSounds() {
  const char* path = "assets/sounds/";
  const char* filenames[] = {"tile_break.wav", "sand.wav", "grass.wav"};

  for (int i = 0; i < sizeof(filenames) / sizeof(char*); i++) {
    char* filepath = (char*)malloc(strlen(path) + strlen(filenames[i]) + 1);
    strcpy(filepath, path);
    strcat(filepath, filenames[i]);
    Sound sound;
    if (loadSound(filepath, sound)) {
      sounds.push_back(sound);
    }
  }
}

void unloadSounds() {
  for (auto& sound : sounds) {
    alDeleteBuffers(1, &sound.buffer);
    alDeleteSources(1, &sound.source);
  }
}
