#include "engine/runtime/realtime_loop.hpp"

#include <utility>

namespace engine::runtime {

RealtimeLoop::RealtimeLoop(Server& server, const time::IClock& clock, std::chrono::nanoseconds interval, Sleeper sleeper)
    : server_(server), clock_(clock), interval_(interval), sleeper_(std::move(sleeper)) {}

foundation::Result<LoopReport> RealtimeLoop::run(const StopCondition& stop, std::uint64_t maxTicks) {
    if (interval_ <= std::chrono::nanoseconds::zero() || !sleeper_) {
        return foundation::Result<LoopReport>::fail(
            foundation::Error{foundation::ErrorCode::InvalidArgument, "intervalle ou fonction d'attente invalide"});
    }
    if (!stop && maxTicks == 0) {
        return foundation::Result<LoopReport>::fail(foundation::Error{foundation::ErrorCode::InvalidArgument, "aucune condition d'arret"});
    }
    LoopReport report;
    std::chrono::nanoseconds deadline = clock_.now() + interval_;
    while ((maxTicks == 0 || report.ticks < maxTicks) && !(stop && stop())) {
        const auto ticked = server_.tick();
        if (!ticked.hasValue()) {
            return foundation::Result<LoopReport>::fail(ticked.error());
        }
        ++report.ticks;
        const std::chrono::nanoseconds now = clock_.now();
        if (now > deadline) {
            ++report.overruns;
            if (now - deadline > report.maxOverrun) {
                report.maxOverrun = now - deadline;
            }
            deadline = now + interval_;
            continue;
        }
        sleeper_(deadline - now);
        deadline += interval_;
    }
    return foundation::Result<LoopReport>::ok(report);
}

}
