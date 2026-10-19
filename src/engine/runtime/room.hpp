#pragma once

#include <cstddef>
#include <cstdint>
#include <set>
#include <utility>

#include "engine/commands/client_id.hpp"
#include "engine/commands/command_processor.hpp"
#include "engine/events/event_bus.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/runtime/room_events.hpp"
#include "engine/runtime/simulation.hpp"
#include "engine/time/api/fixed_timestep.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::runtime {

inline constexpr std::size_t kMaxRoomMembers = 16;

struct RoomReport {
    std::uint32_t steps = 0;
    std::uint32_t eventsDispatched = 0;
    std::uint64_t systemFaults = 0;
    commands::ProcessReport commands;
};

class Room final : private foundation::NonCopyable {
public:
    Room(RoomId id, time::FixedTimestep timestep);
    ~Room();

    [[nodiscard]] RoomId id() const noexcept;
    [[nodiscard]] foundation::Status start();
    [[nodiscard]] foundation::Status stop();
    [[nodiscard]] foundation::Status addSystem(Phase phase, System system);

    [[nodiscard]] foundation::Status join(commands::ClientId client);
    [[nodiscard]] foundation::Status leave(commands::ClientId client);
    [[nodiscard]] bool hasMember(commands::ClientId client) const;
    [[nodiscard]] std::size_t memberCount() const noexcept;
    [[nodiscard]] std::set<commands::ClientId> members() const;

    template <typename Command>
    [[nodiscard]] foundation::Status submit(commands::ClientId client, Command command);

    [[nodiscard]] foundation::Result<RoomReport> tick(const time::IClock& clock);

    [[nodiscard]] SimulationState state() const noexcept;
    [[nodiscard]] commands::CommandProcessor& commands() noexcept;
    [[nodiscard]] events::EventBus& events() noexcept;
    [[nodiscard]] ecs::World& world() noexcept;
    [[nodiscard]] const ecs::World& world() const noexcept;

private:
    RoomId id_;
    Simulation simulation_;
    events::EventBus events_;
    commands::CommandProcessor commands_;
    std::set<commands::ClientId> members_;
};

}

#include "engine/runtime/room.tpp"
