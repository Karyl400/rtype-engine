#pragma once

#include <cstddef>
#include <memory>
#include <string_view>

#include "engine/audio/api/iaudio_engine.hpp"

namespace engine::audio {

class SfmlAudioEngine final : public IAudioEngine {
public:
    SfmlAudioEngine();
    ~SfmlAudioEngine() override;

    [[nodiscard]] foundation::Status play(SoundHandle sound, AudioBus bus, float volume) override;
    [[nodiscard]] foundation::Status setBusVolume(AudioBus bus, float volume) override;
    void setMuted(bool muted) override;

    [[nodiscard]] foundation::Status loadSound(SoundHandle handle, std::string_view filePath);
    [[nodiscard]] foundation::Status createTone(SoundHandle handle, unsigned int frequencyHz, unsigned int durationMs);

    [[nodiscard]] bool hasSound(SoundHandle handle) const;
    [[nodiscard]] bool isMuted() const;
    [[nodiscard]] float busVolume(AudioBus bus) const;
    [[nodiscard]] std::size_t activeSoundCount() const;
    [[nodiscard]] std::size_t playCount() const;

    void stopAll();
    void update();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
