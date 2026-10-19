#pragma once

#include <cstddef>

#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

class NullTransport final : public INetworkTransport {
public:
    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::size_t sentCount() const;
    [[nodiscard]] std::size_t sentBytes() const;

private:
    std::size_t sent_ = 0;
    std::size_t sentBytes_ = 0;
};

}
