#pragma once

#include <span>
#include <vector>

#include "engine/foundation/rect.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

class Camera2D {
public:
    Camera2D();
    Camera2D(foundation::Vec2 center, foundation::Vec2 viewportSize, float zoom = 1.0F);

    void setCenter(foundation::Vec2 center);
    [[nodiscard]] foundation::Vec2 center() const;

    void setViewportSize(foundation::Vec2 size);
    [[nodiscard]] foundation::Vec2 viewportSize() const;

    void setZoom(float zoom);
    [[nodiscard]] float zoom() const;

    void move(foundation::Vec2 offset);

    [[nodiscard]] foundation::Vec2 worldToScreen(foundation::Vec2 worldPos) const;
    [[nodiscard]] foundation::Vec2 screenToWorld(foundation::Vec2 screenPos) const;

    [[nodiscard]] foundation::Rect visibleBounds() const;
    [[nodiscard]] bool isVisible(const foundation::Rect& worldBounds) const;

    [[nodiscard]] RenderCommand apply(const RenderCommand& worldCommand) const;
    void apply(std::span<const RenderCommand> worldCommands, std::vector<RenderCommand>& outScreenCommands) const;

private:
    foundation::Vec2 center_{0.0F, 0.0F};
    foundation::Vec2 viewportSize_{800.0F, 600.0F};
    float zoom_{1.0F};
};

}
