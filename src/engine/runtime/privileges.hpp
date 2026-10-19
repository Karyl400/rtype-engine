#pragma once

#include "engine/foundation/result.hpp"

namespace engine::runtime {

[[nodiscard]] foundation::Status ensureUnprivileged();

}
