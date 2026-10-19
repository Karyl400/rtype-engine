#pragma once

#include <chrono>
#include <cstdint>

namespace engine::runtime {

class TickMetrics final {
public:
    explicit TickMetrics(std::chrono::nanoseconds expectedInterval) noexcept;

    void record(std::chrono::nanoseconds clockNow, std::chrono::nanoseconds processing) noexcept;

    [[nodiscard]] std::uint64_t ticks() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds lastInterval() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds maxInterval() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds maxJitter() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds lastProcessing() const noexcept;
    [[nodiscard]] std::chrono::nanoseconds maxProcessing() const noexcept;

private:
    std::chrono::nanoseconds expected_;
    std::chrono::nanoseconds previous_{0};
    std::chrono::nanoseconds lastInterval_{0};
    std::chrono::nanoseconds maxInterval_{0};
    std::chrono::nanoseconds maxJitter_{0};
    std::chrono::nanoseconds lastProcessing_{0};
    std::chrono::nanoseconds maxProcessing_{0};
    std::uint64_t ticks_ = 0;
};

}
