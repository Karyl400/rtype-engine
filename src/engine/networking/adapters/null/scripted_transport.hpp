#pragma once

#include <cstddef>
#include <deque>
#include <optional>
#include <vector>

#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

class ScriptedTransport final : public INetworkTransport {
public:
    void pushDatagram(std::vector<std::byte> payload, Endpoint from = Endpoint{"127.0.0.1", 0});
    void pushDatagramWithAnnouncedSize(std::vector<std::byte> payload, std::size_t announced);
    void pushFailure();

    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::size_t pending() const noexcept;
    [[nodiscard]] std::size_t sentCount() const noexcept;
    [[nodiscard]] const std::vector<std::vector<std::byte>>& sent() const noexcept;

private:
    struct Entry {
        std::vector<std::byte> payload;
        Endpoint from;
        std::optional<std::size_t> announcedSize;
        bool failure = false;
    };

    std::deque<Entry> queue_;
    std::vector<std::vector<std::byte>> sent_;
};

}
