#include "engine/input/adapters/null/null_input.hpp"

namespace engine::input {

void NullInput::update() {}

bool NullInput::isDown(InputAction) const {
    return false;
}

bool NullInput::wasPressed(InputAction) const {
    return false;
}

}
