#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>

#include "engine/commands/command_processor.hpp"
#include "engine/events/event_bus.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/networking/api/inetwork_transport.hpp"
#include "engine/runtime/simulation.hpp"
#include "engine/runtime/snapshot.hpp"
#include "engine/runtime/tick_metrics.hpp"
#include "engine/time/api/fixed_timestep.hpp"
#include "engine/time/api/iclock.hpp"
#include "engine/tooling/api/iprofiler.hpp"

namespace engine::runtime {

inline constexpr std::size_t kReceiveBufferSize = 2048;
inline constexpr std::uint32_t kMaxDatagramsPerTick = 256;

struct TickReport {
    std::uint32_t steps = 0;
    std::uint32_t datagrams = 0;
    std::uint32_t transportErrors = 0;
    std::uint32_t handlerFaults = 0;
    std::uint64_t systemFaults = 0;
    std::uint32_t eventsDispatched = 0;
    commands::ProcessReport commands;
};

using DatagramHandler = std::function<void(const networking::ReceivedDatagram&, std::span<const std::byte>)>;

class Server final : private foundation::NonCopyable {
public:
    Server(time::FixedTimestep timestep, networking::INetworkTransport& transport, const time::IClock& clock);
    ~Server();

    [[nodiscard]] foundation::Status setHandler(DatagramHandler handler);
    [[nodiscard]] foundation::Status addSystem(Phase phase, System system);
    [[nodiscard]] foundation::Status start();
    [[nodiscard]] foundation::Status stop();
    [[nodiscard]] foundation::Result<TickReport> tick();
    void setProfiler(tooling::IProfiler* profiler) noexcept;
    [[nodiscard]] std::uint64_t profilerFaults() const noexcept;

    [[nodiscard]] SimulationState state() const noexcept;
    [[nodiscard]] std::uint64_t stepCount() const noexcept;
    [[nodiscard]] const TickMetrics& metrics() const noexcept;
    [[nodiscard]] events::EventBus& events() noexcept;
    [[nodiscard]] commands::CommandProcessor& commands() noexcept;
    [[nodiscard]] Snapshot snapshot(const SnapshotRecorder& recorder) const;
    [[nodiscard]] ecs::World& world() noexcept;
    [[nodiscard]] const ecs::World& world() const noexcept;

private:
    using Polled = foundation::Result<std::optional<networking::ReceivedDatagram>>;

    [[nodiscard]] Polled pollTransport();
    void drain(TickReport& report);
    void deliver(const networking::ReceivedDatagram& datagram, TickReport& report);
    void profileBegin();
    void profileEnd(const TickReport* report);

    Simulation simulation_;
    networking::INetworkTransport& transport_;
    const time::IClock& clock_;
    DatagramHandler handler_;
    tooling::IProfiler* profiler_ = nullptr;
    std::uint64_t profilerFaults_ = 0;
    bool zoneOpen_ = false;
    TickMetrics metrics_;
    events::EventBus events_;
    commands::CommandProcessor commands_;
    std::array<std::byte, kReceiveBufferSize> buffer_{};
};

}
