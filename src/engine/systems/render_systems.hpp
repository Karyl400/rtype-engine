#pragma once

#include <cstddef>
#include <vector>

#include "engine/ecs/world.hpp"
#include "engine/foundation/color.hpp"
#include "engine/foundation/rect.hpp"
#include "engine/foundation/result.hpp"
#include "engine/rendering/api/irenderer.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::systems {

struct Sprite {
    rendering::TextureHandle texture{};
    foundation::Rect source{};
    foundation::Color tint{};
    rendering::Layer layer{};
};

struct RenderReport {
    std::size_t submitted = 0;
    std::size_t rejected = 0;
};

[[nodiscard]] foundation::Status validate(const rendering::RenderCommand& command);
[[nodiscard]] std::vector<rendering::RenderCommand> collectRenderCommands(ecs::World& world);
[[nodiscard]] RenderReport renderFrame(ecs::World& world, rendering::IRenderer& renderer);

}
