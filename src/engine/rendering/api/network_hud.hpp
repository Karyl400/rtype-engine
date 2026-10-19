#pragma once

#include <cstdint>
#include <vector>

#include "engine/foundation/color.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

struct NetworkStats {
    float latencyMs = 0.0F;
    float jitterMs = 0.0F;
    float packetLossPercent = 0.0F;
    std::uint64_t serverTick = 0;
};

struct NetworkHudConfig {
    foundation::Vec2 position{10.0F, 10.0F};
    foundation::Vec2 size{180.0F, 40.0F};
    foundation::Color background{20, 24, 32, 220};
    Layer layer{200};
    TextureHandle texture{1};
};

class NetworkHud {
public:
    NetworkHud();
    explicit NetworkHud(const NetworkHudConfig& config);

    void setStats(const NetworkStats& stats);
    [[nodiscard]] const NetworkStats& stats() const;

    void setPosition(foundation::Vec2 position);
    [[nodiscard]] foundation::Vec2 position() const;

    void setVisible(bool visible);
    [[nodiscard]] bool isVisible() const;

    void generateCommands(std::vector<RenderCommand>& outCommands) const;
    [[nodiscard]] std::vector<RenderCommand> generateCommands() const;

private:
    NetworkHudConfig config_{};
    NetworkStats stats_{};
    bool visible_{true};
};

}
