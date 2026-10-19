#include "engine/rendering/api/player_presentation.hpp"

namespace engine::rendering {

foundation::Color playerColor(PlayerIndex index) {
    switch (index) {
        case PlayerIndex::Player1:
            return foundation::Color{230, 50, 50, 255};
        case PlayerIndex::Player2:
            return foundation::Color{40, 140, 240, 255};
        case PlayerIndex::Player3:
            return foundation::Color{240, 210, 30, 255};
        case PlayerIndex::Player4:
            return foundation::Color{40, 220, 80, 255};
    }
    return foundation::Color{255, 255, 255, 255};
}

foundation::Color playerExhaustColor(PlayerIndex index) {
    switch (index) {
        case PlayerIndex::Player1:
            return foundation::Color{255, 120, 50, 220};
        case PlayerIndex::Player2:
            return foundation::Color{80, 200, 255, 220};
        case PlayerIndex::Player3:
            return foundation::Color{255, 240, 100, 220};
        case PlayerIndex::Player4:
            return foundation::Color{100, 255, 150, 220};
    }
    return foundation::Color{255, 200, 100, 200};
}

std::string_view playerName(PlayerIndex index) {
    switch (index) {
        case PlayerIndex::Player1:
            return "Player 1";
        case PlayerIndex::Player2:
            return "Player 2";
        case PlayerIndex::Player3:
            return "Player 3";
        case PlayerIndex::Player4:
            return "Player 4";
    }
    return "Unknown Player";
}

RenderCommand createPlayerShipCommand(PlayerIndex index,
                                      TextureHandle texture,
                                      foundation::Vec2 position,
                                      float scale) {
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.transform.position = position;
    cmd.transform.scale = foundation::Vec2{scale, scale};
    cmd.tint = playerColor(index);
    cmd.source = foundation::Rect{0.0F, 0.0F, 32.0F, 16.0F};
    cmd.layer = Layer{10};
    return cmd;
}

RenderCommand createPlayerExhaustCommand(PlayerIndex index,
                                         TextureHandle texture,
                                         foundation::Vec2 position,
                                         float scale) {
    RenderCommand cmd;
    cmd.texture = texture;
    cmd.transform.position = foundation::Vec2{position.x - 12.0F * scale, position.y + 4.0F * scale};
    cmd.transform.scale = foundation::Vec2{scale, scale};
    cmd.tint = playerExhaustColor(index);
    cmd.source = foundation::Rect{0.0F, 0.0F, 12.0F, 8.0F};
    cmd.layer = Layer{9};
    return cmd;
}

}
