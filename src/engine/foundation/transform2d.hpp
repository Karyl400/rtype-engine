#pragma once

#include "engine/foundation/vec2.hpp"

namespace engine::foundation {

struct Transform2D {
    Vec2 position{};
    float rotationRadians = 0.0F;
    Vec2 scale{1.0F, 1.0F};

    friend bool operator==(const Transform2D&, const Transform2D&) = default;
};

}
