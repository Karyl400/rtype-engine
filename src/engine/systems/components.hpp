#pragma once

#include <chrono>

#include "engine/foundation/vec2.hpp"

namespace engine::systems {

struct Position {
    foundation::Vec2 value;
};

struct Velocity {
    foundation::Vec2 value;
};

struct Lifetime {
    std::chrono::nanoseconds remaining{0};
};

struct Expired {};

}
