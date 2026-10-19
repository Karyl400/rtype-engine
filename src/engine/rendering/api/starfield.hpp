#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "engine/foundation/color.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

struct StarLayerConfig {
    float speed = 40.0F;
    std::size_t starCount = 30;
    foundation::Color tint{255, 255, 255, 255};
    foundation::Vec2 starSize{2.0F, 2.0F};
    Layer layer{0};
};

struct StarfieldConfig {
    foundation::Vec2 bounds{800.0F, 600.0F};
    TextureHandle texture{1};
    std::vector<StarLayerConfig> layers{};
};

struct Star {
    foundation::Vec2 position{};
    foundation::Vec2 size{};
    foundation::Color tint{};
    Layer layer{};
    float speed = 0.0F;
};

class Starfield {
public:
    Starfield();
    explicit Starfield(const StarfieldConfig& config, std::uint32_t seed = 1337);

    void reset(const StarfieldConfig& config, std::uint32_t seed = 1337);
    void update(float dtSeconds);
    void update(std::chrono::nanoseconds dt);

    void setScrollDirection(foundation::Vec2 direction);
    [[nodiscard]] foundation::Vec2 scrollDirection() const;

    void setSpeedMultiplier(float multiplier);
    [[nodiscard]] float speedMultiplier() const;

    [[nodiscard]] std::size_t starCount() const;
    [[nodiscard]] const std::vector<Star>& stars() const;

    void generateCommands(std::vector<RenderCommand>& outCommands) const;
    [[nodiscard]] std::vector<RenderCommand> generateCommands() const;

    [[nodiscard]] static StarfieldConfig defaultRTypeConfig(TextureHandle texture);

private:
    StarfieldConfig config_{};
    std::vector<Star> stars_{};
    foundation::Vec2 scrollDirection_{-1.0F, 0.0F};
    float speedMultiplier_{1.0F};
    std::uint32_t rngState_{1337};

    float nextRandomFloat();
};

}
