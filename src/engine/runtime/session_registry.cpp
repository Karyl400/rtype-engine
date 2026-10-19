#include "engine/runtime/session_registry.hpp"

namespace engine::runtime {

std::string SessionRegistry::keyOf(const networking::Endpoint& endpoint) {
    return endpoint.address + ":" + std::to_string(endpoint.port);
}

foundation::Result<Admission> SessionRegistry::admit(const networking::Endpoint& endpoint) {
    const std::string key = keyOf(endpoint);
    const auto known = byEndpoint_.find(key);
    if (known != byEndpoint_.end()) {
        return foundation::Result<Admission>::ok(Admission{known->second, false});
    }
    if (byEndpoint_.size() >= kMaxClients) {
        return foundation::Result<Admission>::fail(foundation::Error{foundation::ErrorCode::Closed, "nombre maximal de clients atteint"});
    }
    const commands::ClientId client{nextId_++};
    byEndpoint_.emplace(key, client);
    byClient_.emplace(client, endpoint);
    return foundation::Result<Admission>::ok(Admission{client, true});
}

foundation::Status SessionRegistry::release(commands::ClientId client) {
    const auto found = byClient_.find(client);
    if (found == byClient_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "client inconnu");
    }
    byEndpoint_.erase(keyOf(found->second));
    byClient_.erase(found);
    return foundation::okStatus();
}

std::optional<networking::Endpoint> SessionRegistry::endpointOf(commands::ClientId client) const {
    const auto found = byClient_.find(client);
    if (found == byClient_.end()) {
        return std::nullopt;
    }
    return found->second;
}

std::size_t SessionRegistry::size() const noexcept {
    return byClient_.size();
}

}
