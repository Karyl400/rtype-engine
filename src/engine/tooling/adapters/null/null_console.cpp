#include "engine/tooling/adapters/null/null_console.hpp"

#include <string>

namespace engine::tooling {

void NullConsole::log(LogLevel, std::string_view) {}

foundation::Status NullConsole::execute(std::string_view line) {
    return foundation::failStatus(foundation::ErrorCode::NotFound, "commande inconnue : " + std::string{line});
}

}
