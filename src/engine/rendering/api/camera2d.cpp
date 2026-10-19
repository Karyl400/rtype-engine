#include "engine/rendering/api/camera2d.hpp"

#include <cmath>

namespace engine::rendering {

Camera2D::Camera2D() = default;

Camera2D::Camera2D(foundation::Vec2 center, foundation::Vec2 viewportSize, float zoom)
    : center_(center),
      viewportSize_(viewportSize),
      zoom_((zoom > 0.0F && std::isfinite(zoom)) ? zoom : 1.0F) {
    if (viewportSize_.x <= 0.0F || viewportSize_.y <= 0.0F) {
        viewportSize_ = foundation::Vec2{800.0F, 600.0F};
    }
}

void Camera2D::setCenter(foundation::Vec2 center) {
    center_ = center;
}

foundation::Vec2 Camera2D::center() const {
    return center_;
}

void Camera2D::setViewportSize(foundation::Vec2 size) {
    if (size.x > 0.0F && size.y > 0.0F && std::isfinite(size.x) && std::isfinite(size.y)) {
        viewportSize_ = size;
    }
}

foundation::Vec2 Camera2D::viewportSize() const {
    return viewportSize_;
}

void Camera2D::setZoom(float zoom) {
    if (zoom > 0.0F && std::isfinite(zoom)) {
        zoom_ = zoom;
    }
}

float Camera2D::zoom() const {
    return zoom_;
}

void Camera2D::move(foundation::Vec2 offset) {
    center_ = center_ + offset;
}

foundation::Vec2 Camera2D::worldToScreen(foundation::Vec2 worldPos) const {
    const foundation::Vec2 offset = worldPos - center_;
    const foundation::Vec2 screenCenter = viewportSize_ * 0.5F;
    return (offset * zoom_) + screenCenter;
}

foundation::Vec2 Camera2D::screenToWorld(foundation::Vec2 screenPos) const {
    const foundation::Vec2 screenCenter = viewportSize_ * 0.5F;
    const foundation::Vec2 offset = screenPos - screenCenter;
    return (offset * (1.0F / zoom_)) + center_;
}

foundation::Rect Camera2D::visibleBounds() const {
    const float halfWidth = (viewportSize_.x * 0.5F) / zoom_;
    const float halfHeight = (viewportSize_.y * 0.5F) / zoom_;
    return foundation::Rect{
        center_.x - halfWidth,
        center_.y - halfHeight,
        halfWidth * 2.0F,
        halfHeight * 2.0F
    };
}

bool Camera2D::isVisible(const foundation::Rect& worldBounds) const {
    const foundation::Rect camBounds = visibleBounds();
    const bool separateX = (worldBounds.x + worldBounds.width < camBounds.x) || (worldBounds.x > camBounds.x + camBounds.width);
    const bool separateY = (worldBounds.y + worldBounds.height < camBounds.y) || (worldBounds.y > camBounds.y + camBounds.height);
    return !separateX && !separateY;
}

RenderCommand Camera2D::apply(const RenderCommand& worldCommand) const {
    RenderCommand screenCmd = worldCommand;
    screenCmd.transform.position = worldToScreen(worldCommand.transform.position);
    screenCmd.transform.scale = worldCommand.transform.scale * zoom_;
    return screenCmd;
}

void Camera2D::apply(std::span<const RenderCommand> worldCommands, std::vector<RenderCommand>& outScreenCommands) const {
    outScreenCommands.reserve(outScreenCommands.size() + worldCommands.size());
    for (const auto& cmd : worldCommands) {
        if (cmd.source.width > 0.0F && cmd.source.height > 0.0F) {
            const float width = cmd.source.width * std::abs(cmd.transform.scale.x);
            const float height = cmd.source.height * std::abs(cmd.transform.scale.y);
            const foundation::Rect bounds{cmd.transform.position.x, cmd.transform.position.y, width, height};
            if (!isVisible(bounds)) {
                continue;
            }
        }
        outScreenCommands.push_back(apply(cmd));
    }
}

}
