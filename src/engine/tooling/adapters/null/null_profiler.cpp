#include "engine/tooling/adapters/null/null_profiler.hpp"

namespace engine::tooling {

void NullProfiler::beginZone(std::string_view) {}

void NullProfiler::endZone() {}

void NullProfiler::frameMark() {}

void NullProfiler::plot(std::string_view, double) {}

}
