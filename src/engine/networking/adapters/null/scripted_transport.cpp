#include "engine/networking/adapters/null/scripted_transport.hpp"

#include <algorithm>
#include <utility>

namespace engine::networking {

void ScriptedTransport::pushDatagram(std::vector<std::byte> payload, Endpoint from) {
    queue_.push_back(Entry{std::move(payload), std::move(from), std::nullopt, false});
}

void ScriptedTransport::pushDatagramWithAnnouncedSize(std::vector<std::byte> payload, std::size_t announced) {
    queue_.push_back(Entry{std::move(payload), Endpoint{"127.0.0.1", 0}, announced, false});
}

void ScriptedTransport::pushFailure() {
    queue_.push_back(Entry{{}, Endpoint{}, std::nullopt, true});
}

foundation::Status ScriptedTransport::send(const Endpoint&, std::span<const std::byte> payload) {
    sent_.emplace_back(payload.begin(), payload.end());
    return foundation::okStatus();
}

foundation::Result<std::optional<ReceivedDatagram>> ScriptedTransport::poll(std::span<std::byte> buffer) {
    using Out = foundation::Result<std::optional<ReceivedDatagram>>;
    if (queue_.empty()) {
        return Out::ok(std::nullopt);
    }
    Entry next = std::move(queue_.front());
    queue_.pop_front();
    if (next.failure) {
        return Out::fail(foundation::Error{foundation::ErrorCode::Io, "panne simulee du transport"});
    }
    const std::size_t copied = std::min(next.payload.size(), buffer.size());
    std::copy_n(next.payload.begin(), copied, buffer.begin());
    const std::size_t size = next.announcedSize.value_or(copied);
    return Out::ok(ReceivedDatagram{std::move(next.from), size});
}

std::size_t ScriptedTransport::pending() const noexcept {
    return queue_.size();
}

std::size_t ScriptedTransport::sentCount() const noexcept {
    return sent_.size();
}

const std::vector<std::vector<std::byte>>& ScriptedTransport::sent() const noexcept {
    return sent_;
}

}
