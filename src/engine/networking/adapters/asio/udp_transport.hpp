#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>

#include "engine/foundation/result.hpp"
#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

inline constexpr std::size_t kMaxSkippedDatagramsPerPoll = 64;

class UdpTransport final : public INetworkTransport {
    struct Key {
        explicit Key() = default;
    };

public:
    struct Impl;

    [[nodiscard]] static foundation::Result<std::unique_ptr<UdpTransport>> open(const Endpoint& local);

    UdpTransport(Key key, std::unique_ptr<Impl> impl) noexcept;
    ~UdpTransport() override;

    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::uint16_t localPort() const noexcept;

private:
    std::unique_ptr<Impl> impl_;
};

}
