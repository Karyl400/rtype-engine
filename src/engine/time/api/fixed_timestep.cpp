#include "engine/time/api/fixed_timestep.hpp"

namespace engine::time {

FixedTimestep::FixedTimestep(std::chrono::nanoseconds step, std::chrono::nanoseconds maxFrame) : step_(step), maxFrame_(maxFrame) {}

foundation::Result<FixedTimestep> FixedTimestep::create(std::chrono::nanoseconds step, std::chrono::nanoseconds maxFrame) {
    if (step <= std::chrono::nanoseconds::zero()) {
        return foundation::Result<FixedTimestep>::fail(
            foundation::Error{foundation::ErrorCode::InvalidArgument, "pas de temps nul ou negatif"});
    }
    if (maxFrame < step) {
        return foundation::Result<FixedTimestep>::fail(
            foundation::Error{foundation::ErrorCode::InvalidArgument, "duree maximale inferieure au pas"});
    }
    return foundation::Result<FixedTimestep>::ok(FixedTimestep(step, maxFrame));
}

std::uint32_t FixedTimestep::update(const IClock& clock) {
    const std::chrono::nanoseconds now = clock.now();
    if (!started_) {
        started_ = true;
        last_ = now;
        return 0;
    }
    std::chrono::nanoseconds frame = now - last_;
    last_ = now;
    if (frame < std::chrono::nanoseconds::zero()) {
        frame = std::chrono::nanoseconds::zero();
    }
    if (frame > maxFrame_) {
        dropped_ += frame - maxFrame_;
        frame = maxFrame_;
    }
    accumulator_ += frame;
    std::uint32_t steps = 0;
    while (accumulator_ >= step_) {
        accumulator_ -= step_;
        ++steps;
    }
    totalSteps_ += steps;
    return steps;
}

void FixedTimestep::resync() noexcept {
    started_ = false;
    accumulator_ = std::chrono::nanoseconds::zero();
}

float FixedTimestep::alpha() const noexcept {
    return static_cast<float>(accumulator_.count()) / static_cast<float>(step_.count());
}

std::chrono::nanoseconds FixedTimestep::step() const noexcept {
    return step_;
}

std::chrono::nanoseconds FixedTimestep::dropped() const noexcept {
    return dropped_;
}

std::uint64_t FixedTimestep::totalSteps() const noexcept {
    return totalSteps_;
}

}
