#include "engine/runtime/profiling.hpp"

#include <chrono>

namespace engine::runtime {

namespace {

double microseconds(std::chrono::nanoseconds value) {
    return std::chrono::duration<double, std::micro>(value).count();
}

}

void publishTickMetrics(const TickMetrics& metrics, const TickReport& report, tooling::IProfiler& profiler) {
    profiler.plot("tick.steps", static_cast<double>(report.steps));
    profiler.plot("tick.datagrams", static_cast<double>(report.datagrams));
    profiler.plot("tick.events", static_cast<double>(report.eventsDispatched));
    profiler.plot("tick.faults", static_cast<double>(report.systemFaults + report.handlerFaults + report.transportErrors));
    profiler.plot("tick.interval_us", microseconds(metrics.lastInterval()));
    profiler.plot("tick.jitter_us", microseconds(metrics.maxJitter()));
    profiler.plot("tick.processing_us", microseconds(metrics.lastProcessing()));
}

}
