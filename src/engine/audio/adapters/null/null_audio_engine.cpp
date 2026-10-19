#include "engine/audio/adapters/null/null_audio_engine.hpp"

namespace engine::audio {

foundation::Status NullAudioEngine::play(SoundHandle, AudioBus, float volume) {
    if (!validVolume(volume)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "volume hors de [0, 1]");
    }
    if (!muted_) {
        ++played_;
    }
    return foundation::okStatus();
}

foundation::Status NullAudioEngine::setBusVolume(AudioBus bus, float volume) {
    if (!validVolume(volume)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "volume hors de [0, 1]");
    }
    busVolumes_.at(busIndex(bus)) = volume;
    return foundation::okStatus();
}

void NullAudioEngine::setMuted(bool muted) {
    muted_ = muted;
}

std::size_t NullAudioEngine::playCount() const {
    return played_;
}

bool NullAudioEngine::isMuted() const {
    return muted_;
}

float NullAudioEngine::busVolume(AudioBus bus) const {
    return busVolumes_.at(busIndex(bus));
}

bool NullAudioEngine::validVolume(float volume) {
    return volume >= 0.0F && volume <= 1.0F;
}

std::size_t NullAudioEngine::busIndex(AudioBus bus) {
    return static_cast<std::size_t>(bus);
}

}
