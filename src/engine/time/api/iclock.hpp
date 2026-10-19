#pragma once

#include <chrono>

#include "engine/foundation/noncopyable.hpp"

namespace engine::time {

class IClock : private foundation::NonCopyable {
public:
    virtual ~IClock();
    [[nodiscard]] virtual std::chrono::nanoseconds now() const = 0;

protected:
    IClock();
};

}
