#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::networking {

struct Endpoint {
    std::string address;
    std::uint16_t port = 0;
};

struct ReceivedDatagram {
    Endpoint from;
    std::size_t size = 0;
};

class INetworkTransport : private foundation::NonCopyable {
public:
    virtual ~INetworkTransport();
    [[nodiscard]] virtual foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) = 0;
    [[nodiscard]] virtual foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) = 0;

protected:
    INetworkTransport();
};

}
