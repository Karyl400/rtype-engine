#include "engine/rendering/api/starfield.hpp"

#include <cmath>

namespace engine::rendering {

namespace {

constexpr float kMinDirectionLength = 0.0001F;

}

Starfield::Starfield()
    : Starfield(defaultRTypeConfig(TextureHandle{1})) {}

Starfield::Starfield(const StarfieldConfig& config, std::uint32_t seed) {
    reset(config, seed);
}

void Starfield::reset(const StarfieldConfig& config, std::uint32_t seed) {
    config_ = config;
    rngState_ = (seed != 0U) ? seed : 1337U;
    stars_.clear();

    std::size_t total = 0;
    for (const auto& layer : config_.layers) {
        total += layer.starCount;
    }
    stars_.reserve(total);

    for (const auto& layer : config_.layers) {
        for (std::size_t i = 0; i < layer.starCount; ++i) {
            Star star;
            star.position = foundation::Vec2{
                nextRandomFloat() * config_.bounds.x,
                nextRandomFloat() * config_.bounds.y
            };
            star.size = layer.starSize;
            star.tint = layer.tint;
            star.layer = layer.layer;
            star.speed = layer.speed;
            stars_.push_back(star);
        }
    }
}

void Starfield::update(float dtSeconds) {
    if (dtSeconds <= 0.0F || !std::isfinite(dtSeconds)) {
        return;
    }

    for (auto& star : stars_) {
        const foundation::Vec2 displacement = scrollDirection_ * (star.speed * speedMultiplier_ * dtSeconds);
        star.position = star.position + displacement;

        if (config_.bounds.x > 0.0F) {
            if (star.position.x < 0.0F) {
                star.position.x = std::fmod(star.position.x, config_.bounds.x);
                if (star.position.x < 0.0F) {
                    star.position.x += config_.bounds.x;
                }
                star.position.y = nextRandomFloat() * config_.bounds.y;
            } else if (star.position.x >= config_.bounds.x) {
                star.position.x = std::fmod(star.position.x, config_.bounds.x);
                star.position.y = nextRandomFloat() * config_.bounds.y;
            }
        }

        if (config_.bounds.y > 0.0F) {
            if (star.position.y < 0.0F) {
                star.position.y = std::fmod(star.position.y, config_.bounds.y);
                if (star.position.y < 0.0F) {
                    star.position.y += config_.bounds.y;
                }
            } else if (star.position.y >= config_.bounds.y) {
                star.position.y = std::fmod(star.position.y, config_.bounds.y);
            }
        }
    }
}

void Starfield::update(std::chrono::nanoseconds dt) {
    const float dtSec = std::chrono::duration<float>(dt).count();
    update(dtSec);
}

void Starfield::setScrollDirection(foundation::Vec2 direction) {
    const float len = foundation::length(direction);
    if (len > kMinDirectionLength && std::isfinite(len)) {
        scrollDirection_ = foundation::normalized(direction);
    }
}

foundation::Vec2 Starfield::scrollDirection() const {
    return scrollDirection_;
}

void Starfield::setSpeedMultiplier(float multiplier) {
    if (multiplier >= 0.0F && std::isfinite(multiplier)) {
        speedMultiplier_ = multiplier;
    }
}

float Starfield::speedMultiplier() const {
    return speedMultiplier_;
}

std::size_t Starfield::starCount() const {
    return stars_.size();
}

const std::vector<Star>& Starfield::stars() const {
    return stars_;
}

void Starfield::generateCommands(std::vector<RenderCommand>& outCommands) const {
    outCommands.reserve(outCommands.size() + stars_.size());
    for (const auto& star : stars_) {
        RenderCommand cmd;
        cmd.texture = config_.texture;
        cmd.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
        cmd.transform.position = star.position;
        cmd.transform.scale = star.size;
        cmd.transform.rotationRadians = 0.0F;
        cmd.tint = star.tint;
        cmd.layer = star.layer;
        outCommands.push_back(cmd);
    }
}

std::vector<RenderCommand> Starfield::generateCommands() const {
    std::vector<RenderCommand> commands;
    generateCommands(commands);
    return commands;
}

StarfieldConfig Starfield::defaultRTypeConfig(TextureHandle texture) {
    StarfieldConfig config;
    config.bounds = foundation::Vec2{800.0F, 600.0F};
    config.texture = texture;

    config.layers = {
        StarLayerConfig{25.0F, 50, foundation::Color{120, 130, 180, 180}, foundation::Vec2{1.5F, 1.5F}, Layer{0}},
        StarLayerConfig{60.0F, 35, foundation::Color{180, 200, 240, 220}, foundation::Vec2{2.0F, 2.0F}, Layer{0}},
        StarLayerConfig{120.0F, 20, foundation::Color{255, 255, 255, 255}, foundation::Vec2{3.0F, 3.0F}, Layer{0}}
    };

    return config;
}

float Starfield::nextRandomFloat() {
    rngState_ = (rngState_ * 1664525U + 1013904223U);
    return static_cast<float>(rngState_ & 0x00FFFFFFU) / static_cast<float>(0x01000000U);
}

}
