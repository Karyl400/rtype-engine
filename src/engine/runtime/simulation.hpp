#pragma once

#include <chrono>
#include <cstdint>

#include "engine/ecs/world.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/runtime/scheduler.hpp"
#include "engine/time/api/fixed_timestep.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::runtime {

enum class SimulationState {
    Created,
    Running,
    Paused,
    Stopped,
};

[[nodiscard]] const char* simulationStateName(SimulationState state);

class Simulation final : private foundation::NonCopyable {
public:
    explicit Simulation(time::FixedTimestep timestep);
    ~Simulation();

    [[nodiscard]] foundation::Status addSystem(System system);
    [[nodiscard]] foundation::Status addSystem(Phase phase, System system);
    [[nodiscard]] foundation::Status start();
    [[nodiscard]] foundation::Status pause();
    [[nodiscard]] foundation::Status stop();
    [[nodiscard]] foundation::Result<std::uint32_t> update(const time::IClock& clock);

    [[nodiscard]] SimulationState state() const noexcept;
    [[nodiscard]] std::uint64_t stepCount() const noexcept;
    [[nodiscard]] float alpha() const noexcept;
    [[nodiscard]] const Scheduler& scheduler() const noexcept;
    [[nodiscard]] ecs::World& world() noexcept;
    [[nodiscard]] const ecs::World& world() const noexcept;

private:
    time::FixedTimestep timestep_;
    ecs::World world_;
    Scheduler scheduler_;
    SimulationState state_ = SimulationState::Created;
};

}
