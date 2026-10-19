#include "engine/runtime/network_bridge.hpp"

#include <utility>

#include "engine/runtime/session_events.hpp"

namespace engine::runtime {

NetworkBridge::NetworkBridge(Server& server, networking::INetworkTransport& transport, PacketDecoder decoder)
    : server_(server), transport_(transport), decoder_(std::move(decoder)) {}

NetworkBridge::~NetworkBridge() = default;

foundation::Status NetworkBridge::install() {
    if (!decoder_) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "decodeur vide");
    }
    return server_.setHandler(
        [this](const networking::ReceivedDatagram& datagram, std::span<const std::byte> payload) { onDatagram(datagram, payload); });
}

void NetworkBridge::onDatagram(const networking::ReceivedDatagram& datagram, std::span<const std::byte> payload) {
    const auto admission = sessions_.admit(datagram.from);
    if (!admission.hasValue()) {
        ++stats_.refusedClients;
        return;
    }
    const Admission& admitted = admission.value();
    if (admitted.created) {
        server_.events().publish(ClientConnected{admitted.client, datagram.from});
    }
    const foundation::Status decoded = decoder_(admitted.client, payload, server_.commands());
    if (decoded.hasValue()) {
        ++stats_.accepted;
    } else {
        ++stats_.malformed;
    }
}

foundation::Status NetworkBridge::disconnect(commands::ClientId client) {
    foundation::Status released = sessions_.release(client);
    if (!released.hasValue()) {
        return released;
    }
    static_cast<void>(server_.commands().dropClient(client));
    server_.events().publish(ClientDisconnected{client});
    return foundation::okStatus();
}

foundation::Status NetworkBridge::send(commands::ClientId client, std::span<const std::byte> payload) {
    const auto endpoint = sessions_.endpointOf(client);
    if (!endpoint.has_value()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "client inconnu");
    }
    return transport_.send(*endpoint, payload);
}

const BridgeStats& NetworkBridge::stats() const noexcept {
    return stats_;
}

const SessionRegistry& NetworkBridge::sessions() const noexcept {
    return sessions_;
}

}
