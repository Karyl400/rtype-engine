#pragma once

#include <chrono>
#include <cstdint>

#include "engine/foundation/color.hpp"
#include "engine/foundation/rect.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

enum class ProjectileType : std::uint8_t {
    StandardPlasma,
    ChargedBeam,
    Missile
};

struct ProjectileFx {
    ProjectileType type = ProjectileType::StandardPlasma;
    foundation::Vec2 position{};
    foundation::Vec2 velocity{};
    float charge = 1.0F;
    std::chrono::nanoseconds age{0};
    foundation::Color customTint{255, 255, 255, 255};
};

[[nodiscard]] foundation::Color defaultProjectileColor(ProjectileType type);
[[nodiscard]] foundation::Rect projectileSourceRect(ProjectileType type);

[[nodiscard]] RenderCommand createProjectileCommand(const ProjectileFx& fx,
                                                    TextureHandle texture);

[[nodiscard]] RenderCommand createProjectileGlowCommand(const ProjectileFx& fx,
                                                        TextureHandle texture);

}
