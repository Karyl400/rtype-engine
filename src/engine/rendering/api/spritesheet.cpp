#include "engine/rendering/api/spritesheet.hpp"

#include <algorithm>

namespace engine::rendering {

SpriteSheet::SpriteSheet() = default;

SpriteSheet::SpriteSheet(TextureHandle texture, float frameWidth, float frameHeight, std::uint32_t columns, std::uint32_t rows, float spacing)
    : texture_(texture),
      frameWidth_(frameWidth),
      frameHeight_(frameHeight),
      columns_(columns),
      rows_(rows),
      spacing_(spacing) {}

TextureHandle SpriteSheet::texture() const {
    return texture_;
}

float SpriteSheet::frameWidth() const {
    return frameWidth_;
}

float SpriteSheet::frameHeight() const {
    return frameHeight_;
}

std::uint32_t SpriteSheet::columns() const {
    return columns_;
}

std::uint32_t SpriteSheet::rows() const {
    return rows_;
}

std::uint32_t SpriteSheet::totalFrames() const {
    return columns_ * rows_;
}

float SpriteSheet::spacing() const {
    return spacing_;
}

foundation::Rect SpriteSheet::frameRect(std::uint32_t frameIndex) const {
    const std::uint32_t count = totalFrames();
    if (count == 0 || columns_ == 0 || rows_ == 0) {
        return foundation::Rect{0.0F, 0.0F, 0.0F, 0.0F};
    }
    const std::uint32_t clampedIndex = std::min(frameIndex, count - 1);
    const std::uint32_t col = clampedIndex % columns_;
    const std::uint32_t row = clampedIndex / columns_;
    const float x = static_cast<float>(col) * (frameWidth_ + spacing_);
    const float y = static_cast<float>(row) * (frameHeight_ + spacing_);
    return foundation::Rect{x, y, frameWidth_, frameHeight_};
}

}
