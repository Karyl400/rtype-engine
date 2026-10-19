#include "engine/networking/adapters/asio/udp_transport.hpp"

#include <algorithm>
#include <string>
#include <system_error>
#include <utility>

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>

namespace engine::networking {

struct UdpTransport::Impl {
    asio::io_context context;
    asio::ip::udp::socket socket{context};
};

namespace {

using Polled = foundation::Result<std::optional<ReceivedDatagram>>;

constexpr const char* kInvalidAddressMessage = "adresse invalide";
constexpr const char* kOpenFailedMessage = "ouverture du socket impossible";
constexpr const char* kBindFailedMessage = "liaison du socket impossible";
constexpr const char* kSendFailedMessage = "envoi impossible";
constexpr const char* kPartialSendMessage = "envoi incomplet";
constexpr const char* kReceiveFailedMessage = "reception impossible";
constexpr const char* kUnavailableMessage = "transport udp indisponible";

using Opened = foundation::Result<std::unique_ptr<UdpTransport::Impl>>;

Opened openFailure(foundation::ErrorCode code, const char* message) {
    return Opened::fail(foundation::Error{code, message});
}

bool isNothingPending(const std::error_code& error) noexcept {
    return error == asio::error::would_block || error == asio::error::try_again;
}

bool isSkippable(const std::error_code& error) noexcept {
    return error == asio::error::message_size || error == asio::error::connection_reset || error == asio::error::connection_refused;
}

foundation::Result<asio::ip::udp::endpoint> toAsio(const Endpoint& endpoint) {
    std::error_code error;
    const asio::ip::address address = asio::ip::make_address(endpoint.address, error);
    if (error) {
        return foundation::Result<asio::ip::udp::endpoint>::fail(
            foundation::Error{foundation::ErrorCode::InvalidArgument, kInvalidAddressMessage});
    }
    return foundation::Result<asio::ip::udp::endpoint>::ok(asio::ip::udp::endpoint(address, endpoint.port));
}

Opened bindSocket(const Endpoint& local) {
    const auto endpoint = toAsio(local);
    if (!endpoint.hasValue()) {
        return Opened::fail(endpoint.error());
    }
    auto impl = std::make_unique<UdpTransport::Impl>();
    std::error_code error;
    impl->socket.open(endpoint.value().protocol(), error);
    if (error) {
        return openFailure(foundation::ErrorCode::Io, kOpenFailedMessage);
    }
    impl->socket.non_blocking(true, error);
    if (error) {
        return openFailure(foundation::ErrorCode::Io, kOpenFailedMessage);
    }
    impl->socket.bind(endpoint.value(), error);
    if (error) {
        return openFailure(foundation::ErrorCode::Io, kBindFailedMessage);
    }
    return Opened::ok(std::move(impl));
}

}

foundation::Result<std::unique_ptr<UdpTransport>> UdpTransport::open(const Endpoint& local) {
    using Created = foundation::Result<std::unique_ptr<UdpTransport>>;
    try {
        Opened opened = bindSocket(local);
        if (!opened.hasValue()) {
            return Created::fail(opened.error());
        }
        return Created::ok(std::make_unique<UdpTransport>(Key{}, std::move(opened).value()));
    } catch (...) {
        return Created::fail(foundation::Error{foundation::ErrorCode::Io, kUnavailableMessage});
    }
}

UdpTransport::UdpTransport(Key, std::unique_ptr<Impl> impl) noexcept : impl_(std::move(impl)) {}

UdpTransport::~UdpTransport() {
    if (impl_) {
        std::error_code ignored;
        impl_->socket.close(ignored);
    }
}

foundation::Status UdpTransport::send(const Endpoint& to, std::span<const std::byte> payload) {
    const auto endpoint = toAsio(to);
    if (!endpoint.hasValue()) {
        return foundation::failStatus(endpoint.error().code, endpoint.error().message);
    }
    std::error_code error;
    const std::size_t sent = impl_->socket.send_to(asio::buffer(payload.data(), payload.size()), endpoint.value(), 0, error);
    if (error) {
        return foundation::failStatus(foundation::ErrorCode::Io, kSendFailedMessage);
    }
    if (sent != payload.size()) {
        return foundation::failStatus(foundation::ErrorCode::Io, kPartialSendMessage);
    }
    return foundation::okStatus();
}

Polled UdpTransport::poll(std::span<std::byte> buffer) {
    for (std::size_t skipped = 0; skipped < kMaxSkippedDatagramsPerPoll; ++skipped) {
        asio::ip::udp::endpoint sender;
        std::error_code error;
        const std::size_t received = impl_->socket.receive_from(asio::buffer(buffer.data(), buffer.size()), sender, 0, error);
        if (isNothingPending(error)) {
            return Polled::ok(std::nullopt);
        }
        if (isSkippable(error)) {
            continue;
        }
        if (error) {
            return Polled::fail(foundation::Error{foundation::ErrorCode::Io, kReceiveFailedMessage});
        }
        const std::size_t size = std::min(received, buffer.size());
        return Polled::ok(ReceivedDatagram{Endpoint{sender.address().to_string(), sender.port()}, size});
    }
    return Polled::ok(std::nullopt);
}

std::uint16_t UdpTransport::localPort() const noexcept {
    std::error_code error;
    const asio::ip::udp::endpoint local = impl_->socket.local_endpoint(error);
    if (error) {
        return 0;
    }
    return local.port();
}

}
