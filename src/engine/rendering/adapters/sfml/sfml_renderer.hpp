#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include "engine/foundation/color.hpp"
#include "engine/foundation/result.hpp"
#include "engine/rendering/adapters/sfml/sfml_window.hpp"
#include "engine/rendering/api/irenderer.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

class SfmlRenderer final : public IRenderer {
public:
    SfmlRenderer();
    explicit SfmlRenderer(SfmlWindow& window);
    ~SfmlRenderer() override;

    void beginFrame() override;
    void submit(std::span<const RenderCommand> commands) override;
    void endFrame() override;

    [[nodiscard]] std::uint64_t frameCount() const;
    [[nodiscard]] std::uint64_t commandCount() const;

    [[nodiscard]] foundation::Status loadTexture(TextureHandle handle, std::string_view path);
    [[nodiscard]] foundation::Status createSolidTexture(TextureHandle handle, std::uint32_t width, std::uint32_t height, foundation::Color color);
    [[nodiscard]] bool hasTexture(TextureHandle handle) const;
    void clearTextures();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
