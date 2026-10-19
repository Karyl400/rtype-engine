#pragma once

#include <cstdint>

#include "engine/foundation/color.hpp"
#include "engine/foundation/rect.hpp"
#include "engine/foundation/strong_id.hpp"
#include "engine/foundation/transform2d.hpp"

namespace engine::rendering {

struct TextureTag {};
struct LayerTag {};

using TextureHandle = foundation::StrongId<TextureTag>;
using Layer = foundation::StrongId<LayerTag, std::uint8_t>;

struct RenderCommand {
    TextureHandle texture{};
    foundation::Transform2D transform{};
    foundation::Color tint{};
    foundation::Rect source{};
    Layer layer{};
};

}
