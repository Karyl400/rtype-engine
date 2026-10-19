#include "engine/time/adapters/null/manual_clock.hpp"

namespace engine::time {

std::chrono::nanoseconds ManualClock::now() const {
    return now_;
}

foundation::Status ManualClock::advance(std::chrono::nanoseconds delta) {
    if (delta < std::chrono::nanoseconds::zero()) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "delta negatif");
    }
    now_ += delta;
    return foundation::okStatus();
}

}
