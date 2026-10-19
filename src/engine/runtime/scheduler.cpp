#include "engine/runtime/scheduler.hpp"

#include <algorithm>
#include <utility>

namespace engine::runtime {

namespace {

[[nodiscard]] std::size_t indexOf(Phase phase) noexcept {
    return static_cast<std::size_t>(phase);
}

}

const char* phaseName(Phase phase) {
    switch (phase) {
        case Phase::Input: return "Input";
        case Phase::Simulation: return "Simulation";
        case Phase::Replication: return "Replication";
    }
    return "Unknown";
}

foundation::Status Scheduler::add(Phase phase, System system) {
    if (indexOf(phase) >= kPhaseCount) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "phase inconnue");
    }
    if (!system) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "systeme vide");
    }
    phases_[indexOf(phase)].push_back(Entry{std::move(system), 0, false});
    return foundation::okStatus();
}

void Scheduler::runEntry(Entry& entry, ecs::World& world, std::chrono::nanoseconds step) {
    try {
        entry.system(world, step);
        entry.consecutiveFaults = 0;
    } catch (...) {
        ++faults_;
        ++entry.consecutiveFaults;
        entry.disabled = entry.consecutiveFaults >= kMaxConsecutiveFaults;
    }
}

void Scheduler::run(ecs::World& world, std::chrono::nanoseconds step) {
    for (std::vector<Entry>& entries : phases_) {
        for (Entry& entry : entries) {
            if (!entry.disabled) {
                runEntry(entry, world, step);
            }
        }
    }
}

std::size_t Scheduler::size() const noexcept {
    std::size_t total = 0;
    for (const std::vector<Entry>& entries : phases_) {
        total += entries.size();
    }
    return total;
}

std::size_t Scheduler::size(Phase phase) const noexcept {
    if (indexOf(phase) >= kPhaseCount) {
        return 0;
    }
    return phases_[indexOf(phase)].size();
}

std::uint64_t Scheduler::faultCount() const noexcept {
    return faults_;
}

std::size_t Scheduler::disabledCount() const noexcept {
    std::size_t total = 0;
    for (const std::vector<Entry>& entries : phases_) {
        total += static_cast<std::size_t>(std::count_if(entries.begin(), entries.end(), [](const Entry& entry) { return entry.disabled; }));
    }
    return total;
}

}
