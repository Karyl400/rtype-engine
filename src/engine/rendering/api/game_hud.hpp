#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "engine/foundation/color.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/accessibility.hpp"
#include "engine/rendering/api/player_presentation.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

struct PlayerStatus {
    bool active = false;
    bool alive = true;
    std::uint32_t score = 0;
    std::uint8_t lives = 3;
    float beamCharge = 0.0F;
};

struct GameHudState {
    std::uint32_t currentScore = 0;
    std::uint32_t highScore = 50000;
    std::uint8_t lives = 3;
    float beamCharge = 0.0F;
    std::array<PlayerStatus, 4> teamStatus{};
};

class GameHud {
public:
    GameHud();
    explicit GameHud(TextureHandle texture);

    void setState(const GameHudState& state);
    [[nodiscard]] const GameHudState& state() const;

    void setAccessibility(const AccessibilityManager& accessibility);
    [[nodiscard]] const AccessibilityManager& accessibility() const;

    void generateCommands(std::vector<RenderCommand>& outCommands) const;
    [[nodiscard]] std::vector<RenderCommand> generateCommands() const;

private:
    TextureHandle texture_{};
    GameHudState state_{};
    AccessibilityManager accessibility_{};
};

}
