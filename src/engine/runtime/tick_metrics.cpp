#include "engine/runtime/tick_metrics.hpp"

#include <algorithm>

namespace engine::runtime {

TickMetrics::TickMetrics(std::chrono::nanoseconds expectedInterval) noexcept : expected_(expectedInterval) {}

void TickMetrics::record(std::chrono::nanoseconds clockNow, std::chrono::nanoseconds processing) noexcept {
    lastProcessing_ = processing;
    maxProcessing_ = std::max(maxProcessing_, processing);
    if (ticks_ > 0) {
        lastInterval_ = clockNow - previous_;
        maxInterval_ = std::max(maxInterval_, lastInterval_);
        const std::chrono::nanoseconds deviation = lastInterval_ - expected_;
        maxJitter_ = std::max(maxJitter_, deviation < std::chrono::nanoseconds::zero() ? -deviation : deviation);
    }
    previous_ = clockNow;
    ++ticks_;
}

std::uint64_t TickMetrics::ticks() const noexcept {
    return ticks_;
}

std::chrono::nanoseconds TickMetrics::lastInterval() const noexcept {
    return lastInterval_;
}

std::chrono::nanoseconds TickMetrics::maxInterval() const noexcept {
    return maxInterval_;
}

std::chrono::nanoseconds TickMetrics::maxJitter() const noexcept {
    return maxJitter_;
}

std::chrono::nanoseconds TickMetrics::lastProcessing() const noexcept {
    return lastProcessing_;
}

std::chrono::nanoseconds TickMetrics::maxProcessing() const noexcept {
    return maxProcessing_;
}

}
