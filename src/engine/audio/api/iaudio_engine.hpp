#pragma once

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/foundation/strong_id.hpp"

namespace engine::audio {

struct SoundTag {};
using SoundHandle = foundation::StrongId<SoundTag>;

enum class AudioBus { Music, Sfx, Ui };

class IAudioEngine : private foundation::NonCopyable {
public:
    virtual ~IAudioEngine();
    [[nodiscard]] virtual foundation::Status play(SoundHandle sound, AudioBus bus, float volume) = 0;
    [[nodiscard]] virtual foundation::Status setBusVolume(AudioBus bus, float volume) = 0;
    virtual void setMuted(bool muted) = 0;

protected:
    IAudioEngine();
};

}
