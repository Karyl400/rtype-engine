#pragma once

#include "engine/foundation/result.hpp"

namespace engine::runtime {

class ShutdownSignal final {
public:
    [[nodiscard]] static foundation::Status install();
    [[nodiscard]] static bool requested() noexcept;
    static void request() noexcept;
    static void reset() noexcept;

    ShutdownSignal() = delete;
};

}
