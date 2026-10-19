#pragma once

#include <cstdint>

#include "engine/foundation/strong_id.hpp"

namespace engine::ecs {

struct EntityTag {};

using Entity = foundation::StrongId<EntityTag, std::uint32_t>;

}
