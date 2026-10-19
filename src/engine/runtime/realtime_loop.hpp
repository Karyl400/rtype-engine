#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/runtime/server.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::runtime {

struct LoopReport {
    std::uint64_t ticks = 0;
    std::uint64_t overruns = 0;
    std::chrono::nanoseconds maxOverrun{0};
};

using Sleeper = std::function<void(std::chrono::nanoseconds)>;
using StopCondition = std::function<bool()>;

class RealtimeLoop final : private foundation::NonCopyable {
public:
    RealtimeLoop(Server& server, const time::IClock& clock, std::chrono::nanoseconds interval, Sleeper sleeper);

    [[nodiscard]] foundation::Result<LoopReport> run(const StopCondition& stop, std::uint64_t maxTicks);

private:
    Server& server_;
    const time::IClock& clock_;
    std::chrono::nanoseconds interval_;
    Sleeper sleeper_;
};

}
