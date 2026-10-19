#pragma once

#include "engine/foundation/result.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::time {

class ManualClock final : public IClock {
public:
    [[nodiscard]] std::chrono::nanoseconds now() const override;
    [[nodiscard]] foundation::Status advance(std::chrono::nanoseconds delta);

private:
    std::chrono::nanoseconds now_{0};
};

}
