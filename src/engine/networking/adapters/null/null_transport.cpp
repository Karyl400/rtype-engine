#include "engine/networking/adapters/null/null_transport.hpp"

namespace engine::networking {

foundation::Status NullTransport::send(const Endpoint&, std::span<const std::byte> payload) {
    ++sent_;
    sentBytes_ += payload.size();
    return foundation::okStatus();
}

foundation::Result<std::optional<ReceivedDatagram>> NullTransport::poll(std::span<std::byte>) {
    return foundation::Result<std::optional<ReceivedDatagram>>::ok(std::nullopt);
}

std::size_t NullTransport::sentCount() const {
    return sent_;
}

std::size_t NullTransport::sentBytes() const {
    return sentBytes_;
}

}
