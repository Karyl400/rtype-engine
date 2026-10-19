#pragma once

#include <string_view>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::tooling {

enum class LogLevel { Debug, Info, Warning, Error };

class IConsole : private foundation::NonCopyable {
public:
    virtual ~IConsole();
    virtual void log(LogLevel level, std::string_view message) = 0;
    [[nodiscard]] virtual foundation::Status execute(std::string_view line) = 0;

protected:
    IConsole();
};

}
