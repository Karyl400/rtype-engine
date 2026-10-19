#pragma once

#include <cstdint>

#include "engine/commands/client_id.hpp"
#include "engine/foundation/strong_id.hpp"

namespace engine::runtime {

struct RoomTag {};

using RoomId = foundation::StrongId<RoomTag, std::uint32_t>;

struct ClientJoinedRoom {
    RoomId room;
    commands::ClientId client;
};

struct ClientLeftRoom {
    RoomId room;
    commands::ClientId client;
};

}
