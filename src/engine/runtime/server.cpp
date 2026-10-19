#include "engine/runtime/server.hpp"

#include <chrono>
#include <string>
#include <utility>

#include "engine/runtime/profiling.hpp"

namespace engine::runtime {

Server::Server(time::FixedTimestep timestep, networking::INetworkTransport& transport, const time::IClock& clock)
    : simulation_(timestep), transport_(transport), clock_(clock), metrics_(timestep.step()) {}

Server::~Server() = default;

foundation::Status Server::setHandler(DatagramHandler handler) {
    if (simulation_.state() != SimulationState::Created) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "gestionnaire de datagrammes fige apres le demarrage");
    }
    if (!handler) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "gestionnaire vide");
    }
    handler_ = std::move(handler);
    return foundation::okStatus();
}

foundation::Status Server::addSystem(Phase phase, System system) {
    return simulation_.addSystem(phase, std::move(system));
}

foundation::Status Server::start() {
    return simulation_.start();
}

foundation::Status Server::stop() {
    return simulation_.stop();
}

Server::Polled Server::pollTransport() {
    try {
        return transport_.poll(buffer_);
    } catch (...) {
        return Polled::fail(foundation::Error{foundation::ErrorCode::Io, "exception du transport"});
    }
}

void Server::drain(TickReport& report) {
    while (report.datagrams < kMaxDatagramsPerTick) {
        auto polled = pollTransport();
        if (!polled.hasValue()) {
            ++report.transportErrors;
            return;
        }
        const auto& datagram = polled.value();
        if (!datagram.has_value()) {
            return;
        }
        ++report.datagrams;
        deliver(*datagram, report);
    }
}

void Server::deliver(const networking::ReceivedDatagram& datagram, TickReport& report) {
    if (datagram.size > buffer_.size()) {
        ++report.transportErrors;
        return;
    }
    if (!handler_) {
        return;
    }
    try {
        handler_(datagram, std::span<const std::byte>(buffer_.data(), datagram.size));
    } catch (...) {
        ++report.handlerFaults;
    }
}

foundation::Result<TickReport> Server::tick() {
    if (simulation_.state() != SimulationState::Running) {
        return foundation::Result<TickReport>::fail(foundation::Error{
            foundation::ErrorCode::Closed, std::string("tick refuse dans l'etat ") + simulationStateName(simulation_.state())});
    }
    profileBegin();
    const auto began = std::chrono::steady_clock::now();
    const std::uint64_t faultsBefore = simulation_.scheduler().faultCount();
    TickReport report;
    drain(report);
    report.commands = commands_.process(simulation_.world());
    auto steps = simulation_.update(clock_);
    if (!steps.hasValue()) {
        profileEnd(nullptr);
        return foundation::Result<TickReport>::fail(steps.error());
    }
    report.steps = steps.value();
    report.eventsDispatched = static_cast<std::uint32_t>(events_.flush());
    report.systemFaults = simulation_.scheduler().faultCount() - faultsBefore;
    metrics_.record(clock_.now(), std::chrono::steady_clock::now() - began);
    profileEnd(&report);
    return foundation::Result<TickReport>::ok(report);
}

void Server::setProfiler(tooling::IProfiler* profiler) noexcept {
    profiler_ = profiler;
}

std::uint64_t Server::profilerFaults() const noexcept {
    return profilerFaults_;
}

void Server::profileBegin() {
    if (profiler_ == nullptr) {
        return;
    }
    try {
        profiler_->beginZone("server.tick");
        zoneOpen_ = true;
    } catch (...) {
        ++profilerFaults_;
    }
}

void Server::profileEnd(const TickReport* report) {
    if (profiler_ == nullptr) {
        return;
    }
    try {
        if (zoneOpen_) {
            zoneOpen_ = false;
            profiler_->endZone();
        }
        if (report != nullptr) {
            publishTickMetrics(metrics_, *report, *profiler_);
            profiler_->frameMark();
        }
    } catch (...) {
        ++profilerFaults_;
    }
}

SimulationState Server::state() const noexcept {
    return simulation_.state();
}

std::uint64_t Server::stepCount() const noexcept {
    return simulation_.stepCount();
}

const TickMetrics& Server::metrics() const noexcept {
    return metrics_;
}

commands::CommandProcessor& Server::commands() noexcept {
    return commands_;
}

events::EventBus& Server::events() noexcept {
    return events_;
}

Snapshot Server::snapshot(const SnapshotRecorder& recorder) const {
    return recorder.capture(simulation_.world(), simulation_.stepCount());
}

const ecs::World& Server::world() const noexcept {
    return simulation_.world();
}

ecs::World& Server::world() noexcept {
    return simulation_.world();
}

}
