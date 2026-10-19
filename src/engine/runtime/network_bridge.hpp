#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

#include "engine/commands/client_id.hpp"
#include "engine/commands/command_processor.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/networking/api/inetwork_transport.hpp"
#include "engine/runtime/server.hpp"
#include "engine/runtime/session_registry.hpp"

namespace engine::runtime {

using PacketDecoder = std::function<foundation::Status(commands::ClientId, std::span<const std::byte>, commands::CommandProcessor&)>;

struct BridgeStats {
    std::uint64_t accepted = 0;
    std::uint64_t malformed = 0;
    std::uint64_t refusedClients = 0;
};

class NetworkBridge final : private foundation::NonCopyable {
public:
    NetworkBridge(Server& server, networking::INetworkTransport& transport, PacketDecoder decoder);
    ~NetworkBridge();

    [[nodiscard]] foundation::Status install();
    [[nodiscard]] foundation::Status disconnect(commands::ClientId client);
    [[nodiscard]] foundation::Status send(commands::ClientId client, std::span<const std::byte> payload);

    [[nodiscard]] const BridgeStats& stats() const noexcept;
    [[nodiscard]] const SessionRegistry& sessions() const noexcept;

private:
    void onDatagram(const networking::ReceivedDatagram& datagram, std::span<const std::byte> payload);

    Server& server_;
    networking::INetworkTransport& transport_;
    PacketDecoder decoder_;
    SessionRegistry sessions_;
    BridgeStats stats_;
};

}
