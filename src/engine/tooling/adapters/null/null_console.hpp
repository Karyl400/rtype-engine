#pragma once

#include "engine/tooling/api/iconsole.hpp"

namespace engine::tooling {

class NullConsole final : public IConsole {
public:
    void log(LogLevel level, std::string_view message) override;
    [[nodiscard]] foundation::Status execute(std::string_view line) override;
};

}
