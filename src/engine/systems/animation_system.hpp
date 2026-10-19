#pragma once

#include <chrono>
#include <cstdint>

#include "engine/ecs/world.hpp"
#include "engine/rendering/api/spritesheet.hpp"
#include "engine/systems/render_systems.hpp"

namespace engine::systems {

struct AnimationState {
    std::uint32_t startFrame = 0;
    std::uint32_t frameCount = 1;
    std::chrono::nanoseconds frameDuration{100000000};
    std::chrono::nanoseconds elapsedTime{0};
    std::uint32_t currentFrame = 0;
    bool loop = true;
    bool playing = true;
    bool finished = false;
};

void stepAnimation(AnimationState& anim, Sprite& sprite, const rendering::SpriteSheet& sheet, std::chrono::nanoseconds dt);
void resetAnimation(AnimationState& anim);
void updateAnimations(ecs::World& world, std::chrono::nanoseconds dt);

}
