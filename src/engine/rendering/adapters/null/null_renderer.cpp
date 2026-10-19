#include "engine/rendering/adapters/null/null_renderer.hpp"

namespace engine::rendering {

void NullRenderer::beginFrame() {
    ++frames_;
}

void NullRenderer::submit(std::span<const RenderCommand> commands) {
    commands_ += commands.size();
}

void NullRenderer::endFrame() {}

std::size_t NullRenderer::frameCount() const {
    return frames_;
}

std::size_t NullRenderer::commandCount() const {
    return commands_;
}

}
