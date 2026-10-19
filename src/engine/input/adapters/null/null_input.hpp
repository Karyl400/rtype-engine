#pragma once

#include "engine/input/api/iinput.hpp"

namespace engine::input {

class NullInput final : public IInput {
public:
    void update() override;
    [[nodiscard]] bool isDown(InputAction action) const override;
    [[nodiscard]] bool wasPressed(InputAction action) const override;
};

}
