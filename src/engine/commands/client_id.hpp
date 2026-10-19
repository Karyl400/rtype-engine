#pragma once

#include <cstdint>

#include "engine/foundation/strong_id.hpp"

namespace engine::commands {

struct ClientTag {};

using ClientId = foundation::StrongId<ClientTag, std::uint32_t>;

}
