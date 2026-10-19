#pragma once

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/strong_id.hpp"

namespace engine::input {

struct InputActionTag {};

using InputAction = foundation::StrongId<InputActionTag>;

class IInput : private foundation::NonCopyable {
public:
    virtual ~IInput();
    virtual void update() = 0;
    [[nodiscard]] virtual bool isDown(InputAction action) const = 0;
    [[nodiscard]] virtual bool wasPressed(InputAction action) const = 0;

protected:
    IInput();
};

}
