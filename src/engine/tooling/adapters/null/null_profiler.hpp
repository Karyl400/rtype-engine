#pragma once

#include "engine/tooling/api/iprofiler.hpp"

namespace engine::tooling {

class NullProfiler final : public IProfiler {
public:
    void beginZone(std::string_view name) override;
    void endZone() override;
    void frameMark() override;
    void plot(std::string_view name, double value) override;
};

}
