#pragma once

#include <span>

#include "engine/foundation/noncopyable.hpp"
#include "engine/rendering/api/render_command.hpp"

namespace engine::rendering {

class IRenderer : private foundation::NonCopyable {
public:
    virtual ~IRenderer();
    virtual void beginFrame() = 0;
    virtual void submit(std::span<const RenderCommand> commands) = 0;
    virtual void endFrame() = 0;

protected:
    IRenderer();
};

}
