
#ifndef AUDIO_H
#define AUDIO_H

#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class AudioManager {
private:
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::vector<sf::Sound> activeSounds;
    float sfxVolume = 60.0f;
    bool muted = false;

public:
    bool loadSound(const std::string& name, const std::string& path);
    void play(const std::string& name);
    void setVolume(float volume);
    void setMuted(bool state);
};

#endif
