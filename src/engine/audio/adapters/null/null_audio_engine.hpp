#pragma once

#include <array>
#include <cstddef>

#include "engine/audio/api/iaudio_engine.hpp"

namespace engine::audio {

class NullAudioEngine final : public IAudioEngine {
public:
    [[nodiscard]] foundation::Status play(SoundHandle sound, AudioBus bus, float volume) override;
    [[nodiscard]] foundation::Status setBusVolume(AudioBus bus, float volume) override;
    void setMuted(bool muted) override;

    [[nodiscard]] std::size_t playCount() const;
    [[nodiscard]] bool isMuted() const;
    [[nodiscard]] float busVolume(AudioBus bus) const;

private:
    [[nodiscard]] static bool validVolume(float volume);
    [[nodiscard]] static std::size_t busIndex(AudioBus bus);

    std::size_t played_ = 0;
    bool muted_ = false;
    std::array<float, 3> busVolumes_{1.0F, 1.0F, 1.0F};
};

}
