#include "engine/rendering/adapters/sfml/sfml_renderer.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>

namespace engine::rendering {

namespace {

constexpr std::uint32_t kDefaultOffscreenWidth = 800;
constexpr std::uint32_t kDefaultOffscreenHeight = 600;

}

struct SfmlRenderer::Impl {
    SfmlWindow* window{nullptr};
    sf::RenderTexture offscreen{};
    std::unordered_map<TextureHandle, sf::Texture> textures{};
    sf::Texture fallbackTexture{};
    std::uint64_t frameCount{0};
    std::uint64_t commandCount{0};

    explicit Impl(SfmlWindow* win) : window(win) {
        if (window == nullptr) {
            static_cast<void>(offscreen.resize({kDefaultOffscreenWidth, kDefaultOffscreenHeight}));
        }
        const sf::Image img({1, 1}, sf::Color::White);
        static_cast<void>(fallbackTexture.loadFromImage(img));
    }

    sf::RenderTarget& target() {
        if (window != nullptr) {
            auto* renderWin = static_cast<sf::RenderWindow*>(window->nativeHandle());
            return *renderWin;
        }
        return offscreen;
    }
};

SfmlRenderer::SfmlRenderer()
    : impl_(std::make_unique<Impl>(nullptr)) {}

SfmlRenderer::SfmlRenderer(SfmlWindow& window)
    : impl_(std::make_unique<Impl>(&window)) {}

SfmlRenderer::~SfmlRenderer() = default;

void SfmlRenderer::beginFrame() {
    impl_->target().clear(sf::Color::Black);
}

void SfmlRenderer::submit(std::span<const RenderCommand> commands) {
    if (commands.empty()) {
        return;
    }

    std::vector<RenderCommand> sorted(commands.begin(), commands.end());
    std::stable_sort(sorted.begin(), sorted.end(), [](const RenderCommand& a, const RenderCommand& b) {
        return a.layer.value < b.layer.value;
    });

    auto& renderTarget = impl_->target();

    for (const auto& cmd : sorted) {
        const sf::Texture* tex = &impl_->fallbackTexture;
        const auto it = impl_->textures.find(cmd.texture);
        if (it != impl_->textures.end()) {
            tex = &it->second;
        }

        sf::Sprite sprite(*tex);

        if (cmd.source.width > 0.0F && cmd.source.height > 0.0F) {
            const int left = static_cast<int>(std::round(cmd.source.x));
            const int top = static_cast<int>(std::round(cmd.source.y));
            const int width = static_cast<int>(std::round(cmd.source.width));
            const int height = static_cast<int>(std::round(cmd.source.height));
            sprite.setTextureRect(sf::IntRect({left, top}, {width, height}));
        }

        sprite.setPosition({cmd.transform.position.x, cmd.transform.position.y});
        sprite.setRotation(sf::radians(cmd.transform.rotationRadians));
        sprite.setScale({cmd.transform.scale.x, cmd.transform.scale.y});
        sprite.setColor(sf::Color(cmd.tint.r, cmd.tint.g, cmd.tint.b, cmd.tint.a));

        renderTarget.draw(sprite);
        ++impl_->commandCount;
    }
}

void SfmlRenderer::endFrame() {
    if (impl_->window == nullptr) {
        impl_->offscreen.display();
    }
    ++impl_->frameCount;
}

std::uint64_t SfmlRenderer::frameCount() const {
    return impl_->frameCount;
}

std::uint64_t SfmlRenderer::commandCount() const {
    return impl_->commandCount;
}

foundation::Status SfmlRenderer::loadTexture(TextureHandle handle, std::string_view path) {
    sf::Texture tex;
    if (!tex.loadFromFile(std::filesystem::path(path))) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "echec chargement texture");
    }
    impl_->textures[handle] = std::move(tex);
    return foundation::okStatus();
}

foundation::Status SfmlRenderer::createSolidTexture(TextureHandle handle, std::uint32_t width, std::uint32_t height, foundation::Color color) {
    if (width == 0 || height == 0) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "dimensions de texture invalides");
    }
    const sf::Image img({width, height}, sf::Color(color.r, color.g, color.b, color.a));
    sf::Texture tex;
    if (!tex.loadFromImage(img)) {
        return foundation::failStatus(foundation::ErrorCode::Io, "echec creation texture");
    }
    impl_->textures[handle] = std::move(tex);
    return foundation::okStatus();
}

bool SfmlRenderer::hasTexture(TextureHandle handle) const {
    return impl_->textures.contains(handle);
}

void SfmlRenderer::clearTextures() {
    impl_->textures.clear();
}

}
