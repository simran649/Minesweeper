
#include "audio.h"
#include <algorithm>

bool AudioManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(path))
        return false;

    buffers.insert_or_assign(name, std::move(buffer));
    return true;
}

void AudioManager::play(const std::string& name) {
    if (muted)
        return;

    auto found = buffers.find(name);

    if (found == buffers.end())
        return;

    activeSounds.erase(
        std::remove_if(
            activeSounds.begin(),
            activeSounds.end(),
            [](const std::unique_ptr<sf::Sound>& sound) {
                return sound->getStatus() == sf::SoundSource::Status::Stopped;
            }
        ),
        activeSounds.end()
    );

    auto sound = std::make_unique<sf::Sound>(found->second);
    sound->setVolume(volume);
    sound->play();

    activeSounds.push_back(std::move(sound));
}

void AudioManager::setVolume(float value) {
    volume = std::clamp(value, 0.0f, 100.0f);
}

void AudioManager::setMuted(bool value) {
    muted = value;
}
