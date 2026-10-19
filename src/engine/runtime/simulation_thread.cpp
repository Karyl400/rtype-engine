#include "engine/runtime/simulation_thread.hpp"

#include <utility>

namespace engine::runtime {

SimulationThread::SimulationThread(Server& server, const time::IClock& clock, std::chrono::nanoseconds interval, Sleeper sleeper)
    : loop_(server, clock, interval, std::move(sleeper)) {}

SimulationThread::~SimulationThread() {
    requestStop();
    if (thread_.joinable()) {
        thread_.join();
    }
}

foundation::Status SimulationThread::start(std::uint64_t maxTicks) {
    if (started_) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "fil de simulation deja demarre");
    }
    started_ = true;
    thread_ = std::thread([this, maxTicks] { body(maxTicks); });
    return foundation::okStatus();
}

void SimulationThread::requestStop() noexcept {
    stop_.store(true);
}

foundation::Result<LoopReport> SimulationThread::join() {
    if (!started_) {
        return foundation::Result<LoopReport>::fail(
            foundation::Error{foundation::ErrorCode::InvalidArgument, "fil de simulation non demarre"});
    }
    if (thread_.joinable()) {
        thread_.join();
    }
    if (!outcome_.has_value()) {
        return foundation::Result<LoopReport>::fail(foundation::Error{foundation::ErrorCode::Io, "fil de simulation sans resultat"});
    }
    return *outcome_;
}

bool SimulationThread::started() const noexcept {
    return started_;
}

bool SimulationThread::finished() const noexcept {
    return finished_.load();
}

void SimulationThread::body(std::uint64_t maxTicks) {
    try {
        outcome_ = loop_.run([this] { return stop_.load(); }, maxTicks);
    } catch (...) {
        outcome_ =
            foundation::Result<LoopReport>::fail(foundation::Error{foundation::ErrorCode::Io, "exception dans le fil de simulation"});
    }
    finished_.store(true);
}

}
