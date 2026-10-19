#include "engine/runtime/simulation.hpp"

#include <string>
#include <utility>

namespace engine::runtime {

const char* simulationStateName(SimulationState state) {
    switch (state) {
        case SimulationState::Created: return "Created";
        case SimulationState::Running: return "Running";
        case SimulationState::Paused: return "Paused";
        case SimulationState::Stopped: return "Stopped";
    }
    return "Unknown";
}

Simulation::Simulation(time::FixedTimestep timestep) : timestep_(timestep) {}

Simulation::~Simulation() = default;

foundation::Status Simulation::addSystem(System system) {
    return addSystem(Phase::Simulation, std::move(system));
}

foundation::Status Simulation::addSystem(Phase phase, System system) {
    if (state_ != SimulationState::Created) {
        return foundation::failStatus(foundation::ErrorCode::Closed,
                                      std::string("systeme refuse dans l'etat ") + simulationStateName(state_));
    }
    return scheduler_.add(phase, std::move(system));
}

foundation::Status Simulation::start() {
    if (state_ != SimulationState::Created && state_ != SimulationState::Paused) {
        return foundation::failStatus(foundation::ErrorCode::Closed,
                                      std::string("demarrage refuse dans l'etat ") + simulationStateName(state_));
    }
    if (state_ == SimulationState::Paused) {
        timestep_.resync();
    }
    state_ = SimulationState::Running;
    return foundation::okStatus();
}

foundation::Status Simulation::pause() {
    if (state_ != SimulationState::Running) {
        return foundation::failStatus(foundation::ErrorCode::Closed,
                                      std::string("pause refusee dans l'etat ") + simulationStateName(state_));
    }
    state_ = SimulationState::Paused;
    return foundation::okStatus();
}

foundation::Status Simulation::stop() {
    if (state_ == SimulationState::Stopped) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "simulation deja arretee");
    }
    state_ = SimulationState::Stopped;
    world_.clear();
    return foundation::okStatus();
}

foundation::Result<std::uint32_t> Simulation::update(const time::IClock& clock) {
    if (state_ != SimulationState::Running) {
        return foundation::Result<std::uint32_t>::fail(foundation::Error{
            foundation::ErrorCode::Closed, std::string("mise a jour refusee dans l'etat ") + simulationStateName(state_)});
    }
    const std::uint32_t steps = timestep_.update(clock);
    for (std::uint32_t i = 0; i < steps; ++i) {
        scheduler_.run(world_, timestep_.step());
    }
    return foundation::Result<std::uint32_t>::ok(steps);
}

SimulationState Simulation::state() const noexcept {
    return state_;
}

std::uint64_t Simulation::stepCount() const noexcept {
    return timestep_.totalSteps();
}

float Simulation::alpha() const noexcept {
    return timestep_.alpha();
}

const Scheduler& Simulation::scheduler() const noexcept {
    return scheduler_;
}

ecs::World& Simulation::world() noexcept {
    return world_;
}

const ecs::World& Simulation::world() const noexcept {
    return world_;
}

}
