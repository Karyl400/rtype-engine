#pragma once

#include "engine/runtime/server.hpp"
#include "engine/runtime/tick_metrics.hpp"
#include "engine/tooling/api/iprofiler.hpp"

namespace engine::runtime {

void publishTickMetrics(const TickMetrics& metrics, const TickReport& report, tooling::IProfiler& profiler);

}
