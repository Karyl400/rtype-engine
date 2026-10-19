#include "engine/audio/adapters/sfml/sfml_audio_engine.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundChannel.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace engine::audio {

namespace {

constexpr float kPi = 3.14159265358979323846F;
constexpr unsigned int kSampleRate = 44100;
constexpr std::size_t kMaxChannels = 32;

bool validVolume(float volume) {
    return std::isfinite(volume) && volume >= 0.0F && volume <= 1.0F;
}

std::size_t busIndex(AudioBus bus) {
    return static_cast<std::size_t>(bus);
}

}

struct SfmlAudioEngine::Impl {
    struct SoundChannel {
        sf::Sound sound;
        AudioBus bus = AudioBus::Sfx;
        float baseVolume = 1.0F;

        explicit SoundChannel(const sf::SoundBuffer& buffer) : sound(buffer) {}
    };

    std::unordered_map<SoundHandle, sf::SoundBuffer> buffers;
    std::vector<std::unique_ptr<SoundChannel>> channels;
    std::array<float, 3> busVolumes{1.0F, 1.0F, 1.0F};
    bool muted = false;
    std::size_t playCount = 0;

    void pruneStopped() {
        std::erase_if(channels, [](const std::unique_ptr<SoundChannel>& ch) {
            return ch->sound.getStatus() == sf::Sound::Status::Stopped;
        });
    }

    void applyVolumes() {
        for (auto& ch : channels) {
            const float busVol = busVolumes.at(busIndex(ch->bus));
            const float effective = muted ? 0.0F : (ch->baseVolume * busVol * 100.0F);
            ch->sound.setVolume(effective);
        }
    }
};

SfmlAudioEngine::SfmlAudioEngine() : impl_(std::make_unique<Impl>()) {}

SfmlAudioEngine::~SfmlAudioEngine() = default;

foundation::Status SfmlAudioEngine::play(SoundHandle sound, AudioBus bus, float volume) {
    if (!validVolume(volume)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "volume hors de [0, 1]");
    }
    const auto it = impl_->buffers.find(sound);
    if (it == impl_->buffers.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "son introuvable");
    }
    ++impl_->playCount;
    if (impl_->muted) {
        return foundation::okStatus();
    }
    impl_->pruneStopped();
    if (impl_->channels.size() >= kMaxChannels) {
        impl_->channels.front()->sound.stop();
        impl_->channels.erase(impl_->channels.begin());
    }
    auto ch = std::make_unique<Impl::SoundChannel>(it->second);
    ch->bus = bus;
    ch->baseVolume = volume;
    const float busVol = impl_->busVolumes.at(busIndex(bus));
    const float effective = volume * busVol * 100.0F;
    ch->sound.setVolume(effective);
    ch->sound.play();
    impl_->channels.push_back(std::move(ch));
    return foundation::okStatus();
}

foundation::Status SfmlAudioEngine::setBusVolume(AudioBus bus, float volume) {
    if (!validVolume(volume)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "volume hors de [0, 1]");
    }
    impl_->busVolumes.at(busIndex(bus)) = volume;
    impl_->applyVolumes();
    return foundation::okStatus();
}

void SfmlAudioEngine::setMuted(bool muted) {
    impl_->muted = muted;
    impl_->applyVolumes();
}

foundation::Status SfmlAudioEngine::loadSound(SoundHandle handle, std::string_view filePath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(std::filesystem::path(filePath))) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "echec chargement son");
    }
    impl_->buffers[handle] = std::move(buffer);
    return foundation::okStatus();
}

foundation::Status SfmlAudioEngine::createTone(SoundHandle handle, unsigned int frequencyHz, unsigned int durationMs) {
    if (frequencyHz == 0 || durationMs == 0) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "parametres de tonalite invalides");
    }
    const std::size_t sampleCount = static_cast<std::size_t>(kSampleRate) * durationMs / 1000;
    std::vector<std::int16_t> samples(sampleCount);
    for (std::size_t i = 0; i < sampleCount; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(kSampleRate);
        const double wave = std::sin(2.0 * static_cast<double>(kPi) * static_cast<double>(frequencyHz) * t);
        const double envelope = std::min(1.0, std::min(static_cast<double>(i) / 441.0, static_cast<double>(sampleCount - 1 - i) / 441.0));
        samples[i] = static_cast<std::int16_t>(wave * envelope * 16000.0);
    }
    sf::SoundBuffer buffer;
    if (!buffer.loadFromSamples(samples.data(), samples.size(), 1, kSampleRate, {sf::SoundChannel::Mono})) {
        return foundation::failStatus(foundation::ErrorCode::Io, "echec creation buffer audio");
    }
    impl_->buffers[handle] = std::move(buffer);
    return foundation::okStatus();
}

bool SfmlAudioEngine::hasSound(SoundHandle handle) const {
    return impl_->buffers.find(handle) != impl_->buffers.end();
}

bool SfmlAudioEngine::isMuted() const {
    return impl_->muted;
}

float SfmlAudioEngine::busVolume(AudioBus bus) const {
    return impl_->busVolumes.at(busIndex(bus));
}

std::size_t SfmlAudioEngine::activeSoundCount() const {
    std::size_t count = 0;
    for (const auto& ch : impl_->channels) {
        if (ch->sound.getStatus() == sf::Sound::Status::Playing) {
            ++count;
        }
    }
    return count;
}

std::size_t SfmlAudioEngine::playCount() const {
    return impl_->playCount;
}

void SfmlAudioEngine::stopAll() {
    for (auto& ch : impl_->channels) {
        ch->sound.stop();
    }
    impl_->channels.clear();
}

void SfmlAudioEngine::update() {
    impl_->pruneStopped();
}

}
