#pragma once

#include <cstddef>
#include <span>

#include "engine/rendering/api/irenderer.hpp"

namespace engine::rendering {

class NullRenderer final : public IRenderer {
public:
    void beginFrame() override;
    void submit(std::span<const RenderCommand> commands) override;
    void endFrame() override;

    [[nodiscard]] std::size_t frameCount() const;
    [[nodiscard]] std::size_t commandCount() const;

private:
    std::size_t frames_ = 0;
    std::size_t commands_ = 0;
};

}
