#include "engine/rendering/api/projectile_fx.hpp"

#include <algorithm>

namespace engine::rendering {

foundation::Color defaultProjectileColor(ProjectileType type) {
    switch (type) {
        case ProjectileType::StandardPlasma:
            return foundation::Color{255, 230, 80, 255};
        case ProjectileType::ChargedBeam:
            return foundation::Color{80, 200, 255, 255};
        case ProjectileType::Missile:
            return foundation::Color{255, 130, 40, 255};
    }
    return foundation::Color{255, 255, 255, 255};
}

foundation::Rect projectileSourceRect(ProjectileType type) {
    switch (type) {
        case ProjectileType::StandardPlasma:
            return foundation::Rect{0.0F, 0.0F, 8.0F, 4.0F};
        case ProjectileType::ChargedBeam:
            return foundation::Rect{0.0F, 0.0F, 24.0F, 12.0F};
        case ProjectileType::Missile:
            return foundation::Rect{0.0F, 0.0F, 16.0F, 6.0F};
    }
    return foundation::Rect{0.0F, 0.0F, 8.0F, 4.0F};
}

RenderCommand createProjectileCommand(const ProjectileFx& fx, TextureHandle texture) {
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.transform.position = fx.position;
    const float scale = (fx.type == ProjectileType::ChargedBeam) ? std::max(1.0F, fx.charge) : 1.0F;
    cmd.transform.scale = foundation::Vec2{scale, scale};
    if (fx.customTint == foundation::Color{255, 255, 255, 255}) {
        cmd.tint = defaultProjectileColor(fx.type);
    } else {
        cmd.tint = fx.customTint;
    }
    cmd.source = projectileSourceRect(fx.type);
    cmd.layer = Layer{15};
    return cmd;
}

RenderCommand createProjectileGlowCommand(const ProjectileFx& fx, TextureHandle texture) {
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.transform.position = fx.position;
    const float baseScale = (fx.type == ProjectileType::ChargedBeam) ? std::max(1.0F, fx.charge) : 1.0F;
    cmd.transform.scale = foundation::Vec2{baseScale * 1.5F, baseScale * 1.5F};
    foundation::Color c = (fx.customTint == foundation::Color{255, 255, 255, 255}) ? defaultProjectileColor(fx.type) : fx.customTint;
    c.a = 90;
    cmd.tint = c;
    cmd.source = projectileSourceRect(fx.type);
    cmd.layer = Layer{14};
    return cmd;
}

}
