#include "engine/rendering/api/network_hud.hpp"

#include <algorithm>

namespace engine::rendering {

namespace {

constexpr float kLatencyGoodThreshold = 50.0F;
constexpr float kLatencyWarningThreshold = 120.0F;

foundation::Color latencyColor(float latencyMs) {
    if (latencyMs < kLatencyGoodThreshold) {
        return foundation::Color{50, 220, 100, 255};
    }
    if (latencyMs < kLatencyWarningThreshold) {
        return foundation::Color{240, 200, 40, 255};
    }
    return foundation::Color{240, 60, 60, 255};
}

}

NetworkHud::NetworkHud() = default;

NetworkHud::NetworkHud(const NetworkHudConfig& config)
    : config_(config) {}

void NetworkHud::setStats(const NetworkStats& stats) {
    stats_ = stats;
}

const NetworkStats& NetworkHud::stats() const {
    return stats_;
}

void NetworkHud::setPosition(foundation::Vec2 position) {
    config_.position = position;
}

foundation::Vec2 NetworkHud::position() const {
    return config_.position;
}

void NetworkHud::setVisible(bool visible) {
    visible_ = visible;
}

bool NetworkHud::isVisible() const {
    return visible_;
}

void NetworkHud::generateCommands(std::vector<RenderCommand>& outCommands) const {
    if (!visible_) {
        return;
    }

    const auto baseLayer = config_.layer;
    const auto decorLayer = Layer{static_cast<std::uint8_t>(baseLayer.value + 1)};
    const auto contentLayer = Layer{static_cast<std::uint8_t>(baseLayer.value + 2)};

    RenderCommand background{};
    background.texture = config_.texture;
    background.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    background.transform.position = config_.position;
    background.transform.scale = config_.size;
    background.tint = config_.background;
    background.layer = baseLayer;
    outCommands.push_back(background);

    RenderCommand latencyTrack{};
    latencyTrack.texture = config_.texture;
    latencyTrack.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    latencyTrack.transform.position = foundation::Vec2{config_.position.x + 8.0F, config_.position.y + 8.0F};
    latencyTrack.transform.scale = foundation::Vec2{100.0F, 10.0F};
    latencyTrack.tint = foundation::Color{40, 48, 60, 255};
    latencyTrack.layer = decorLayer;
    outCommands.push_back(latencyTrack);

    const float latencyWidth = std::clamp(stats_.latencyMs, 2.0F, 100.0F);
    RenderCommand latencyBar{};
    latencyBar.texture = config_.texture;
    latencyBar.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    latencyBar.transform.position = foundation::Vec2{config_.position.x + 8.0F, config_.position.y + 8.0F};
    latencyBar.transform.scale = foundation::Vec2{latencyWidth, 10.0F};
    latencyBar.tint = latencyColor(stats_.latencyMs);
    latencyBar.layer = contentLayer;
    outCommands.push_back(latencyBar);

    const float jitterWidth = std::clamp(stats_.jitterMs * 2.0F, 2.0F, 25.0F);
    RenderCommand jitterBar{};
    jitterBar.texture = config_.texture;
    jitterBar.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    jitterBar.transform.position = foundation::Vec2{config_.position.x + 116.0F, config_.position.y + 8.0F};
    jitterBar.transform.scale = foundation::Vec2{jitterWidth, 10.0F};
    jitterBar.tint = foundation::Color{100, 180, 255, 255};
    jitterBar.layer = contentLayer;
    outCommands.push_back(jitterBar);

    RenderCommand lossDot{};
    lossDot.texture = config_.texture;
    lossDot.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    lossDot.transform.position = foundation::Vec2{config_.position.x + 150.0F, config_.position.y + 8.0F};
    lossDot.transform.scale = foundation::Vec2{10.0F, 10.0F};
    lossDot.tint = (stats_.packetLossPercent > 0.0F)
        ? foundation::Color{255, 80, 40, 255}
        : foundation::Color{60, 180, 80, 255};
    lossDot.layer = contentLayer;
    outCommands.push_back(lossDot);

    RenderCommand tickBand{};
    tickBand.texture = config_.texture;
    tickBand.source = foundation::Rect{0.0F, 0.0F, 1.0F, 1.0F};
    tickBand.transform.position = foundation::Vec2{config_.position.x + 8.0F, config_.position.y + 24.0F};
    tickBand.transform.scale = foundation::Vec2{164.0F, 6.0F};
    tickBand.tint = ((stats_.serverTick % 2) == 0)
        ? foundation::Color{80, 120, 180, 200}
        : foundation::Color{60, 90, 140, 200};
    tickBand.layer = decorLayer;
    outCommands.push_back(tickBand);
}

std::vector<RenderCommand> NetworkHud::generateCommands() const {
    std::vector<RenderCommand> commands;
    generateCommands(commands);
    return commands;
}

}
