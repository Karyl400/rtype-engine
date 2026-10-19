#include "engine/networking/adapters/null/loopback_transport.hpp"

#include <algorithm>
#include <utility>

namespace engine::networking {

void LoopbackTransport::push(std::vector<std::byte> payload, Endpoint from) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(Entry{std::move(payload), std::move(from), false});
}

void LoopbackTransport::pushFailure() {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(Entry{{}, {}, true});
}

foundation::Status LoopbackTransport::send(const Endpoint&, std::span<const std::byte> payload) {
    std::lock_guard<std::mutex> lock(mutex_);
    sent_.emplace_back(payload.begin(), payload.end());
    return foundation::okStatus();
}

foundation::Result<std::optional<ReceivedDatagram>> LoopbackTransport::poll(std::span<std::byte> buffer) {
    using Out = foundation::Result<std::optional<ReceivedDatagram>>;
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return Out::ok(std::nullopt);
    }
    Entry entry = std::move(queue_.front());
    queue_.pop_front();
    if (entry.failure) {
        return Out::fail(foundation::Error{foundation::ErrorCode::Io, "echec simule"});
    }
    const std::size_t size = std::min(entry.payload.size(), buffer.size());
    std::copy_n(entry.payload.begin(), size, buffer.begin());
    return Out::ok(ReceivedDatagram{std::move(entry.from), size});
}

std::size_t LoopbackTransport::sentCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sent_.size();
}

std::size_t LoopbackTransport::pending() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

}
