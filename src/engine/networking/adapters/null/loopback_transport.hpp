#pragma once

#include <cstddef>
#include <deque>
#include <mutex>
#include <vector>

#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

class LoopbackTransport final : public INetworkTransport {
public:
    void push(std::vector<std::byte> payload, Endpoint from = Endpoint{"127.0.0.1", 4242});
    void pushFailure();

    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::size_t sentCount() const;
    [[nodiscard]] std::size_t pending() const;

private:
    struct Entry {
        std::vector<std::byte> payload;
        Endpoint from;
        bool failure = false;
    };

    mutable std::mutex mutex_;
    std::deque<Entry> queue_;
    std::vector<std::vector<std::byte>> sent_;
};

}
