#include "engine/time/adapters/system/system_clock.hpp"

namespace engine::time {

SystemClock::SystemClock() : origin_(std::chrono::steady_clock::now()) {}

std::chrono::nanoseconds SystemClock::now() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - origin_);
}

}
