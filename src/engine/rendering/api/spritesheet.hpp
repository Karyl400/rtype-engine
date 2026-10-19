#pragma once

#include <cstdint>

#include "engine/foundation/rect.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

class SpriteSheet {
public:
    SpriteSheet();
    SpriteSheet(TextureHandle texture, float frameWidth, float frameHeight, std::uint32_t columns, std::uint32_t rows, float spacing = 0.0F);

    [[nodiscard]] TextureHandle texture() const;
    [[nodiscard]] float frameWidth() const;
    [[nodiscard]] float frameHeight() const;
    [[nodiscard]] std::uint32_t columns() const;
    [[nodiscard]] std::uint32_t rows() const;
    [[nodiscard]] std::uint32_t totalFrames() const;
    [[nodiscard]] float spacing() const;

    [[nodiscard]] foundation::Rect frameRect(std::uint32_t frameIndex) const;

private:
    TextureHandle texture_{};
    float frameWidth_ = 0.0F;
    float frameHeight_ = 0.0F;
    std::uint32_t columns_ = 0;
    std::uint32_t rows_ = 0;
    float spacing_ = 0.0F;
};

}
