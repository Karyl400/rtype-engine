#include "engine/runtime/privileges.hpp"

#include <unistd.h>

namespace engine::runtime {

foundation::Status ensureUnprivileged() {
    if (::geteuid() == 0) {
        return foundation::failStatus(foundation::ErrorCode::PermissionDenied, "le serveur refuse de s'executer avec les privileges root");
    }
    return foundation::okStatus();
}

}
