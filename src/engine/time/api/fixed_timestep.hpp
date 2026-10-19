#pragma once

#include <chrono>
#include <cstdint>

#include "engine/foundation/result.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::time {

class FixedTimestep final {
public:
    [[nodiscard]] static foundation::Result<FixedTimestep> create(std::chrono::nanoseconds step, std::chrono::nanoseconds maxFrame);

    [[nodiscard]] std::uint32_t update(const IClock& clock);
    void resync() noexcept;
    [[nodiscard]] float alpha() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds step() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds dropped() const noexcept;
    [[nodiscard]] std::uint64_t totalSteps() const noexcept;

private:
    FixedTimestep(std::chrono::nanoseconds step, std::chrono::nanoseconds maxFrame);

    std::chrono::nanoseconds step_;
    std::chrono::nanoseconds maxFrame_;
    std::chrono::nanoseconds last_{0};
    std::chrono::nanoseconds accumulator_{0};
    std::chrono::nanoseconds dropped_{0};
    std::uint64_t totalSteps_ = 0;
    bool started_ = false;
};

}
