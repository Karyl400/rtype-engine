#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "engine/ecs/world.hpp"
#include "engine/foundation/result.hpp"

namespace engine::runtime {

enum class Phase {
    Input,
    Simulation,
    Replication,
};

inline constexpr std::size_t kPhaseCount = 3;
inline constexpr std::uint32_t kMaxConsecutiveFaults = 3;

[[nodiscard]] const char* phaseName(Phase phase);

using System = std::function<void(ecs::World&, std::chrono::nanoseconds)>;

class Scheduler final {
public:
    [[nodiscard]] foundation::Status add(Phase phase, System system);
    void run(ecs::World& world, std::chrono::nanoseconds step);
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t size(Phase phase) const noexcept;
    [[nodiscard]] std::uint64_t faultCount() const noexcept;
    [[nodiscard]] std::size_t disabledCount() const noexcept;

private:
    struct Entry {
        System system;
        std::uint32_t consecutiveFaults = 0;
        bool disabled = false;
    };

    void runEntry(Entry& entry, ecs::World& world, std::chrono::nanoseconds step);

    std::array<std::vector<Entry>, kPhaseCount> phases_;
    std::uint64_t faults_ = 0;
};

}
