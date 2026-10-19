#include "engine/rendering/api/game_hud.hpp"

#include <algorithm>

namespace engine::rendering {

GameHud::GameHud() = default;

GameHud::GameHud(TextureHandle texture) : texture_(texture) {}

void GameHud::setState(const GameHudState& state) {
    state_ = state;
}

const GameHudState& GameHud::state() const {
    return state_;
}

void GameHud::setAccessibility(const AccessibilityManager& accessibility) {
    accessibility_ = accessibility;
}

const AccessibilityManager& GameHud::accessibility() const {
    return accessibility_;
}

void GameHud::generateCommands(std::vector<RenderCommand>& outCommands) const {
    const float scale = accessibility_.uiScale();
    const Layer hudLayer{200};

    RenderCommand topBanner;
    topBanner.texture = texture_;
    topBanner.transform.position = foundation::Vec2{10.0F * scale, 10.0F * scale};
    topBanner.transform.scale = foundation::Vec2{scale, scale};
    topBanner.source = foundation::Rect{0.0F, 0.0F, 780.0F, 28.0F};
    topBanner.tint = accessibility_.applyFilter(foundation::Color{10, 15, 30, 200});
    topBanner.layer = hudLayer;
    outCommands.push_back(topBanner);

    RenderCommand scoreTrack;
    scoreTrack.texture = texture_;
    scoreTrack.transform.position = foundation::Vec2{20.0F * scale, 16.0F * scale};
    scoreTrack.transform.scale = foundation::Vec2{scale, scale};
    scoreTrack.source = foundation::Rect{0.0F, 0.0F, 140.0F, 16.0F};
    scoreTrack.tint = accessibility_.applyFilter(foundation::Color{30, 50, 90, 220});
    scoreTrack.layer = hudLayer;
    outCommands.push_back(scoreTrack);

    RenderCommand highScoreTrack;
    highScoreTrack.texture = texture_;
    highScoreTrack.transform.position = foundation::Vec2{300.0F * scale, 16.0F * scale};
    highScoreTrack.transform.scale = foundation::Vec2{scale, scale};
    highScoreTrack.source = foundation::Rect{0.0F, 0.0F, 140.0F, 16.0F};
    highScoreTrack.tint = accessibility_.applyFilter(foundation::Color{70, 60, 30, 220});
    highScoreTrack.layer = hudLayer;
    outCommands.push_back(highScoreTrack);

    for (std::size_t i = 0; i < state_.teamStatus.size(); ++i) {
        const auto& member = state_.teamStatus[i];
        RenderCommand badge;
        badge.texture = texture_;
        badge.transform.position = foundation::Vec2{(560.0F + static_cast<float>(i) * 55.0F) * scale, 16.0F * scale};
        badge.transform.scale = foundation::Vec2{scale, scale};
        badge.source = foundation::Rect{0.0F, 0.0F, 48.0F, 16.0F};

        foundation::Color col = playerColor(static_cast<PlayerIndex>(i));
        if (!member.active) {
            col.a = 70;
        } else if (!member.alive) {
            col = foundation::Color{90, 90, 90, 180};
        }
        badge.tint = accessibility_.applyFilter(col);
        badge.layer = hudLayer;
        outCommands.push_back(badge);
    }

    const float bottomY = 560.0F * scale;
    for (std::uint8_t i = 0; i < state_.lives; ++i) {
        RenderCommand lifeIcon;
        lifeIcon.texture = texture_;
        lifeIcon.transform.position = foundation::Vec2{(20.0F + static_cast<float>(i) * 18.0F) * scale, bottomY};
        lifeIcon.transform.scale = foundation::Vec2{scale, scale};
        lifeIcon.source = foundation::Rect{0.0F, 0.0F, 14.0F, 8.0F};
        lifeIcon.tint = accessibility_.applyFilter(playerColor(PlayerIndex::Player1));
        lifeIcon.layer = hudLayer;
        outCommands.push_back(lifeIcon);
    }

    RenderCommand beamTrack;
    beamTrack.texture = texture_;
    beamTrack.transform.position = foundation::Vec2{100.0F * scale, bottomY};
    beamTrack.transform.scale = foundation::Vec2{scale, scale};
    beamTrack.source = foundation::Rect{0.0F, 0.0F, 200.0F, 10.0F};
    beamTrack.tint = accessibility_.applyFilter(foundation::Color{20, 25, 40, 230});
    beamTrack.layer = hudLayer;
    outCommands.push_back(beamTrack);

    const float clampedCharge = std::clamp(state_.beamCharge, 0.0F, 1.0F);
    if (clampedCharge > 0.0F) {
        RenderCommand beamFill;
        beamFill.texture = texture_;
        beamFill.transform.position = foundation::Vec2{100.0F * scale, bottomY};
        beamFill.transform.scale = foundation::Vec2{scale, scale};
        beamFill.source = foundation::Rect{0.0F, 0.0F, 200.0F * clampedCharge, 10.0F};

        foundation::Color fillCol;
        if (state_.beamCharge >= 1.0F) {
            fillCol = foundation::Color{100, 255, 255, 255};
        } else {
            fillCol = foundation::Color{40, 160, 240, 240};
        }
        beamFill.tint = accessibility_.applyFilter(fillCol);
        beamFill.layer = hudLayer;
        outCommands.push_back(beamFill);
    }
}

std::vector<RenderCommand> GameHud::generateCommands() const {
    std::vector<RenderCommand> commands;
    generateCommands(commands);
    return commands;
}

}
