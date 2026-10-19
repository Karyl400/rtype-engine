#pragma once

#include "engine/commands/client_id.hpp"
#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::runtime {

struct ClientConnected {
    commands::ClientId client;
    networking::Endpoint endpoint;
};

struct ClientDisconnected {
    commands::ClientId client;
};

}
