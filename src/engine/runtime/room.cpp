#include "engine/runtime/room.hpp"

#include <utility>

namespace engine::runtime {

Room::Room(RoomId id, time::FixedTimestep timestep) : id_(id), simulation_(timestep) {}

Room::~Room() = default;

RoomId Room::id() const noexcept {
    return id_;
}

foundation::Status Room::start() {
    return simulation_.start();
}

foundation::Status Room::stop() {
    return simulation_.stop();
}

foundation::Status Room::addSystem(Phase phase, System system) {
    return simulation_.addSystem(phase, std::move(system));
}

foundation::Status Room::join(commands::ClientId client) {
    if (members_.contains(client)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "client deja dans la salle");
    }
    if (members_.size() >= kMaxRoomMembers) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "salle pleine");
    }
    members_.insert(client);
    (void)events_.enqueue(ClientJoinedRoom{id_, client});
    return foundation::okStatus();
}

foundation::Status Room::leave(commands::ClientId client) {
    if (members_.erase(client) == 0) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "client absent de la salle");
    }
    (void)commands_.dropClient(client);
    (void)events_.enqueue(ClientLeftRoom{id_, client});
    return foundation::okStatus();
}

bool Room::hasMember(commands::ClientId client) const {
    return members_.contains(client);
}

std::size_t Room::memberCount() const noexcept {
    return members_.size();
}

std::set<commands::ClientId> Room::members() const {
    return members_;
}

foundation::Result<RoomReport> Room::tick(const time::IClock& clock) {
    RoomReport report;
    const std::uint64_t faultsBefore = simulation_.scheduler().faultCount();
    report.commands = commands_.process(simulation_.world());
    auto steps = simulation_.update(clock);
    if (!steps.hasValue()) {
        return foundation::Result<RoomReport>::fail(steps.error());
    }
    report.steps = steps.value();
    report.eventsDispatched = static_cast<std::uint32_t>(events_.flush());
    report.systemFaults = simulation_.scheduler().faultCount() - faultsBefore;
    return foundation::Result<RoomReport>::ok(report);
}

SimulationState Room::state() const noexcept {
    return simulation_.state();
}

commands::CommandProcessor& Room::commands() noexcept {
    return commands_;
}

events::EventBus& Room::events() noexcept {
    return events_;
}

ecs::World& Room::world() noexcept {
    return simulation_.world();
}

const ecs::World& Room::world() const noexcept {
    return simulation_.world();
}

}
