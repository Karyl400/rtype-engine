#pragma once

#include <string_view>

#include "engine/foundation/noncopyable.hpp"

namespace engine::tooling {

class IProfiler : private foundation::NonCopyable {
public:
    virtual ~IProfiler();
    virtual void beginZone(std::string_view name) = 0;
    virtual void endZone() = 0;
    virtual void frameMark() = 0;
    virtual void plot(std::string_view name, double value) = 0;

protected:
    IProfiler();
};

}
