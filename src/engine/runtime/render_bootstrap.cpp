#include "engine/runtime/render_bootstrap.hpp"

#include <utility>

#include "engine/rendering/adapters/null/null_renderer.hpp"

#if defined(ENGINE_BUILD_SFML_BACKEND)
#include "engine/rendering/adapters/sfml/sfml_renderer.hpp"
#endif

namespace engine::runtime {

foundation::Result<RenderBackend> parseRenderBackend(std::string_view name) {
    if (name == "NULL") {
        return foundation::Result<RenderBackend>::ok(RenderBackend::Null);
    }
    if (name == "SFML") {
        return foundation::Result<RenderBackend>::ok(RenderBackend::Sfml);
    }
    return foundation::Result<RenderBackend>::fail(foundation::Error{foundation::ErrorCode::InvalidArgument, "backend de rendu inconnu"});
}

foundation::Result<std::unique_ptr<rendering::IRenderer>> createRenderer(RenderBackend backend) {
    if (backend == RenderBackend::Null) {
        std::unique_ptr<rendering::IRenderer> renderer = std::make_unique<rendering::NullRenderer>();
        return foundation::Result<std::unique_ptr<rendering::IRenderer>>::ok(std::move(renderer));
    }
#if defined(ENGINE_BUILD_SFML_BACKEND)
    if (backend == RenderBackend::Sfml) {
        std::unique_ptr<rendering::IRenderer> renderer = std::make_unique<rendering::SfmlRenderer>();
        return foundation::Result<std::unique_ptr<rendering::IRenderer>>::ok(std::move(renderer));
    }
#endif
    return foundation::Result<std::unique_ptr<rendering::IRenderer>>::fail(
        foundation::Error{foundation::ErrorCode::Unsupported, "adapter SFML non disponible"});
}

}
