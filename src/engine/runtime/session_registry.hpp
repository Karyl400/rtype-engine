#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "engine/commands/client_id.hpp"
#include "engine/foundation/result.hpp"
#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::runtime {

inline constexpr std::size_t kMaxClients = 64;

struct Admission {
    commands::ClientId client;
    bool created = false;
};

class SessionRegistry final {
public:
    [[nodiscard]] foundation::Result<Admission> admit(const networking::Endpoint& endpoint);
    [[nodiscard]] foundation::Status release(commands::ClientId client);
    [[nodiscard]] std::optional<networking::Endpoint> endpointOf(commands::ClientId client) const;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    [[nodiscard]] static std::string keyOf(const networking::Endpoint& endpoint);

    std::unordered_map<std::string, commands::ClientId> byEndpoint_;
    std::unordered_map<commands::ClientId, networking::Endpoint> byClient_;
    std::uint32_t nextId_ = 1;
};

}
