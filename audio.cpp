
#include "audio.h"
#include <algorithm>

bool AudioManager::loadSound(
    const std::string& name,
    const std::string& path
) {
    auto [it, inserted] = soundBuffers.try_emplace(name);

    if (!it->second.loadFromFile(path)) {
        soundBuffers.erase(it);
        return false;
    }

    return true;
}

void AudioManager::play(const std::string& name) {
    if (muted)
        return;

    auto it = soundBuffers.find(name);

    if (it == soundBuffers.end())
        return;

    std::erase_if(activeSounds, [](const sf::Sound& sound) {
        return sound.getStatus() == sf::Sound::Status::Stopped;
    });

    activeSounds.emplace_back(it->second);
    activeSounds.back().setVolume(sfxVolume);
    activeSounds.back().play();
}

void AudioManager::setVolume(float volume) {
    sfxVolume = std::clamp(volume, 0.0f, 100.0f);
}

void AudioManager::setMuted(bool state) {
    muted = state;
}
