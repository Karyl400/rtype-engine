#pragma once

#include <chrono>

#include "engine/time/api/iclock.hpp"

namespace engine::time {

class SystemClock final : public IClock {
public:
    SystemClock();

    [[nodiscard]] std::chrono::nanoseconds now() const override;

private:
    std::chrono::steady_clock::time_point origin_;
};

}
