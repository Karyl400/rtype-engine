#pragma once

#include <cstdint>
#include <string_view>

#include "engine/foundation/color.hpp"
#include "engine/foundation/vec2.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

enum class PlayerIndex : std::uint8_t {
    Player1 = 0,
    Player2 = 1,
    Player3 = 2,
    Player4 = 3
};

[[nodiscard]] foundation::Color playerColor(PlayerIndex index);
[[nodiscard]] foundation::Color playerExhaustColor(PlayerIndex index);
[[nodiscard]] std::string_view playerName(PlayerIndex index);

[[nodiscard]] RenderCommand createPlayerShipCommand(PlayerIndex index,
                                                    TextureHandle texture,
                                                    foundation::Vec2 position,
                                                    float scale = 1.0F);

[[nodiscard]] RenderCommand createPlayerExhaustCommand(PlayerIndex index,
                                                       TextureHandle texture,
                                                       foundation::Vec2 position,
                                                       float scale = 1.0F);

}
