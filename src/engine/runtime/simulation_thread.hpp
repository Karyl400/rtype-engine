#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <optional>
#include <thread>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/runtime/realtime_loop.hpp"
#include "engine/runtime/server.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::runtime {

class SimulationThread final : private foundation::NonCopyable {
public:
    SimulationThread(Server& server, const time::IClock& clock, std::chrono::nanoseconds interval, Sleeper sleeper);
    ~SimulationThread();

    [[nodiscard]] foundation::Status start(std::uint64_t maxTicks);
    void requestStop() noexcept;
    [[nodiscard]] foundation::Result<LoopReport> join();
    [[nodiscard]] bool started() const noexcept;
    [[nodiscard]] bool finished() const noexcept;

private:
    void body(std::uint64_t maxTicks);

    RealtimeLoop loop_;
    std::atomic<bool> stop_{false};
    std::atomic<bool> finished_{false};
    bool started_ = false;
    std::optional<foundation::Result<LoopReport>> outcome_;
    std::thread thread_;
};

}
