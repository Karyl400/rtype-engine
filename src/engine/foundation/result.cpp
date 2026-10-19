#include "engine/foundation/result.hpp"

namespace engine::foundation {

std::string_view errorCodeName(ErrorCode code) {
    switch (code) {
        case ErrorCode::InvalidArgument: return "InvalidArgument";
        case ErrorCode::NotFound: return "NotFound";
        case ErrorCode::Io: return "Io";
        case ErrorCode::Unsupported: return "Unsupported";
        case ErrorCode::Timeout: return "Timeout";
        case ErrorCode::Closed: return "Closed";
        case ErrorCode::PermissionDenied: return "PermissionDenied";
    }
    return "Unknown";
}

Status okStatus() {
    return Status::ok(Unit{});
}

Status failStatus(ErrorCode code, std::string message) {
    return Status::fail(Error{code, std::move(message)});
}

}
