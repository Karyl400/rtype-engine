#pragma once

#include <cstdint>
#include <vector>

#include "engine/foundation/noncopyable.hpp"

namespace engine::rendering {

struct WindowEvent {
    enum class Type { Closed, Resized, FocusLost, FocusGained };

    Type type = Type::Closed;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

using EventQueue = std::vector<WindowEvent>;

class IWindow : private foundation::NonCopyable {
public:
    virtual ~IWindow();
    [[nodiscard]] virtual bool isOpen() const = 0;
    virtual void pollEvents(EventQueue& queue) = 0;
    virtual void present() = 0;

protected:
    IWindow();
};

}
