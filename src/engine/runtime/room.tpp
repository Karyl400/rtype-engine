#pragma once

namespace engine::runtime {

template <typename Command>
foundation::Status Room::submit(commands::ClientId client, Command command) {
    if (!hasMember(client)) {
        return foundation::failStatus(foundation::ErrorCode::PermissionDenied, "client absent de la salle");
    }
    return commands_.submit(client, std::move(command));
}

}
