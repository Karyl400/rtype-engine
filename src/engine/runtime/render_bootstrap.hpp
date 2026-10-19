#pragma once

#include <memory>
#include <string_view>

#include "engine/foundation/result.hpp"
#include "engine/rendering/api/irenderer.hpp"

namespace engine::runtime {

enum class RenderBackend {
    Null,
    Sfml,
};

[[nodiscard]] foundation::Result<RenderBackend> parseRenderBackend(std::string_view name);
[[nodiscard]] foundation::Result<std::unique_ptr<rendering::IRenderer>> createRenderer(RenderBackend backend);

}
