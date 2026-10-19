#include "engine/systems/animation_system.hpp"

namespace engine::systems {

void stepAnimation(AnimationState& anim, Sprite& sprite, const rendering::SpriteSheet& sheet, std::chrono::nanoseconds dt) {
    if (sheet.totalFrames() == 0 || anim.frameCount == 0) {
        return;
    }
    if (anim.playing && dt > std::chrono::nanoseconds{0} && anim.frameDuration > std::chrono::nanoseconds{0}) {
        anim.elapsedTime += dt;
        while (anim.elapsedTime >= anim.frameDuration) {
            anim.elapsedTime -= anim.frameDuration;
            anim.currentFrame++;
            if (anim.currentFrame >= anim.startFrame + anim.frameCount) {
                if (anim.loop) {
                    anim.currentFrame = anim.startFrame;
                } else {
                    anim.currentFrame = anim.startFrame + anim.frameCount - 1;
                    anim.playing = false;
                    anim.finished = true;
                    break;
                }
            }
        }
    }
    sprite.source = sheet.frameRect(anim.currentFrame);
    sprite.texture = sheet.texture();
}

void resetAnimation(AnimationState& anim) {
    anim.currentFrame = anim.startFrame;
    anim.elapsedTime = std::chrono::nanoseconds{0};
    anim.finished = false;
    anim.playing = true;
}

void updateAnimations(ecs::World& world, std::chrono::nanoseconds dt) {
    world.each<AnimationState, Sprite, rendering::SpriteSheet>([dt](ecs::Entity, AnimationState& anim, Sprite& sprite, rendering::SpriteSheet& sheet) {
        stepAnimation(anim, sprite, sheet, dt);
    });
}

}
