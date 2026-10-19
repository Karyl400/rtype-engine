#include "engine/networking/adapters/null/lossy_transport.hpp"

#include <algorithm>
#include <utility>

namespace engine::networking {

namespace {

constexpr std::uint32_t kPercent = 100;
constexpr std::uint64_t kFallbackSeed = 0x9E3779B97F4A7C15ULL;

}

LossyTransport::LossyTransport(INetworkTransport& inner, LossyProfile profile)
    : inner_(inner), profile_(profile), state_(profile.seed == 0 ? kFallbackSeed : profile.seed) {}

std::uint32_t LossyTransport::draw(std::uint32_t bound) noexcept {
    state_ ^= state_ << 13;
    state_ ^= state_ >> 7;
    state_ ^= state_ << 17;
    return bound == 0 ? 0 : static_cast<std::uint32_t>(state_ % bound);
}

foundation::Status LossyTransport::send(const Endpoint& to, std::span<const std::byte> payload) {
    if (draw(kPercent) < profile_.dropPercent) {
        ++dropped_;
        return foundation::okStatus();
    }
    return inner_.send(to, payload);
}

foundation::Result<std::optional<ReceivedDatagram>> LossyTransport::release(std::span<std::byte> buffer) {
    Held held = std::move(held_.front());
    held_.pop_front();
    const std::size_t size = std::min(held.payload.size(), buffer.size());
    std::copy_n(held.payload.begin(), size, buffer.begin());
    ++delivered_;
    return foundation::Result<std::optional<ReceivedDatagram>>::ok(ReceivedDatagram{std::move(held.from), size});
}

foundation::Result<std::optional<ReceivedDatagram>> LossyTransport::poll(std::span<std::byte> buffer) {
    for (Held& held : held_) {
        if (held.remaining > 0) {
            --held.remaining;
        }
    }
    const auto ready = std::find_if(held_.begin(), held_.end(), [](const Held& held) { return held.remaining == 0; });
    if (ready != held_.end()) {
        std::rotate(held_.begin(), ready, ready + 1);
        return release(buffer);
    }
    while (true) {
        auto polled = inner_.poll(buffer);
        if (!polled.hasValue() || !polled.value().has_value()) {
            return polled;
        }
        const ReceivedDatagram datagram = *polled.value();
        if (draw(kPercent) < profile_.dropPercent) {
            ++dropped_;
            continue;
        }
        const std::vector<std::byte> copy(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(datagram.size));
        if (draw(kPercent) < profile_.duplicatePercent) {
            ++duplicated_;
            held_.push_back(Held{copy, datagram.from, 1 + draw(std::max<std::uint32_t>(profile_.maxDelayPolls, 1))});
        }
        if (draw(kPercent) < profile_.delayPercent) {
            ++delayed_;
            held_.push_back(Held{copy, datagram.from, 1 + draw(std::max<std::uint32_t>(profile_.maxDelayPolls, 1))});
            continue;
        }
        ++delivered_;
        return polled;
    }
}

std::uint64_t LossyTransport::dropped() const noexcept {
    return dropped_;
}

std::uint64_t LossyTransport::duplicated() const noexcept {
    return duplicated_;
}

std::uint64_t LossyTransport::delayed() const noexcept {
    return delayed_;
}

std::uint64_t LossyTransport::delivered() const noexcept {
    return delivered_;
}

}
