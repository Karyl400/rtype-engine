#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>

#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

struct LossyProfile {
    std::uint32_t dropPercent = 0;
    std::uint32_t duplicatePercent = 0;
    std::uint32_t delayPercent = 0;
    std::uint32_t maxDelayPolls = 4;
    std::uint64_t seed = 1;
};

class LossyTransport final : public INetworkTransport {
public:
    LossyTransport(INetworkTransport& inner, LossyProfile profile);

    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::uint64_t dropped() const noexcept;
    [[nodiscard]] std::uint64_t duplicated() const noexcept;
    [[nodiscard]] std::uint64_t delayed() const noexcept;
    [[nodiscard]] std::uint64_t delivered() const noexcept;

private:
    struct Held {
        std::vector<std::byte> payload;
        Endpoint from;
        std::uint32_t remaining = 0;
    };

    [[nodiscard]] std::uint32_t draw(std::uint32_t bound) noexcept;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> release(std::span<std::byte> buffer);

    INetworkTransport& inner_;
    LossyProfile profile_;
    std::uint64_t state_;
    std::deque<Held> held_;
    std::uint64_t dropped_ = 0;
    std::uint64_t duplicated_ = 0;
    std::uint64_t delayed_ = 0;
    std::uint64_t delivered_ = 0;
};

}
