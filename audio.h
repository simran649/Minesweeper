
#ifndef AUDIO_H
#define AUDIO_H

#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AudioManager {
private:
    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::vector<std::unique_ptr<sf::Sound>> activeSounds;

    float volume = 60.0f;
    bool muted = false;

public:
    bool loadSound(const std::string& name, const std::string& path);
    void play(const std::string& name);
    void setVolume(float value);
    void setMuted(bool value);
};

#endif
