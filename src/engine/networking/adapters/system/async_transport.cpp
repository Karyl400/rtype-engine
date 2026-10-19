#include "engine/networking/adapters/system/async_transport.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <utility>

namespace engine::networking {

namespace {

constexpr std::chrono::milliseconds kIdleWait{1};

}

AsyncTransport::AsyncTransport(INetworkTransport& inner) : inner_(inner), thread_([this] { ioLoop(); }) {}

AsyncTransport::~AsyncTransport() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopping_.store(true);
    }
    wake_.notify_all();
    thread_.join();
}

foundation::Status AsyncTransport::send(const Endpoint& to, std::span<const std::byte> payload) {
    if (stopping_.load()) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "transport asynchrone arrete");
    }
    if (payload.size() > kAsyncMaxDatagram) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "datagramme trop grand");
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (outbox_.size() >= kAsyncQueueCapacity) {
            return foundation::failStatus(foundation::ErrorCode::Io, "file sortante pleine");
        }
        outbox_.push_back(Packet{to, std::vector<std::byte>(payload.begin(), payload.end())});
    }
    wake_.notify_one();
    return foundation::okStatus();
}

foundation::Result<std::optional<ReceivedDatagram>> AsyncTransport::poll(std::span<std::byte> buffer) {
    using Out = foundation::Result<std::optional<ReceivedDatagram>>;
    std::lock_guard<std::mutex> lock(mutex_);
    if (pendingErrors_ > 0) {
        --pendingErrors_;
        return Out::fail(foundation::Error{foundation::ErrorCode::Io, "erreur du transport sous-jacent"});
    }
    if (inbox_.empty()) {
        return Out::ok(std::nullopt);
    }
    Packet packet = std::move(inbox_.front());
    inbox_.pop_front();
    if (packet.payload.size() > buffer.size()) {
        return Out::fail(foundation::Error{foundation::ErrorCode::InvalidArgument, "tampon trop petit, datagramme perdu"});
    }
    std::copy(packet.payload.begin(), packet.payload.end(), buffer.begin());
    return Out::ok(ReceivedDatagram{std::move(packet.peer), packet.payload.size()});
}

std::uint64_t AsyncTransport::droppedInbound() const noexcept {
    return dropped_.load();
}

std::uint64_t AsyncTransport::ioErrors() const noexcept {
    return ioErrors_.load();
}

std::uint64_t AsyncTransport::forwarded() const noexcept {
    return forwarded_.load();
}

void AsyncTransport::recordIoError() {
    ioErrors_.fetch_add(1);
    std::lock_guard<std::mutex> lock(mutex_);
    ++pendingErrors_;
}

void AsyncTransport::ioLoop() {
    while (!stopping_.load()) {
        const bool sent = flushOutbound();
        const bool received = pumpInbound();
        if (!sent && !received) {
            std::unique_lock<std::mutex> lock(mutex_);
            wake_.wait_for(lock, kIdleWait, [this] { return stopping_.load() || !outbox_.empty(); });
        }
    }
    static_cast<void>(flushOutbound());
}

bool AsyncTransport::flushOutbound() {
    std::deque<Packet> batch;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        batch.swap(outbox_);
    }
    for (const Packet& packet : batch) {
        try {
            const foundation::Status sent = inner_.send(packet.peer, packet.payload);
            if (sent.hasValue()) {
                forwarded_.fetch_add(1);
            } else {
                ioErrors_.fetch_add(1);
            }
        } catch (...) {
            ioErrors_.fetch_add(1);
        }
    }
    return !batch.empty();
}

bool AsyncTransport::pumpInbound() {
    std::array<std::byte, kAsyncMaxDatagram> scratch{};
    bool any = false;
    for (std::size_t pass = 0; pass < kAsyncMaxPumpPerPass; ++pass) {
        try {
            const auto polled = inner_.poll(scratch);
            if (!polled.hasValue()) {
                recordIoError();
                return any;
            }
            const std::optional<ReceivedDatagram>& received = polled.value();
            if (!received.has_value()) {
                return any;
            }
            const ReceivedDatagram& datagram = *received;
            if (datagram.size > scratch.size()) {
                recordIoError();
                continue;
            }
            any = true;
            std::lock_guard<std::mutex> lock(mutex_);
            if (inbox_.size() >= kAsyncQueueCapacity) {
                dropped_.fetch_add(1);
                continue;
            }
            inbox_.push_back(Packet{datagram.from,
                                    std::vector<std::byte>(scratch.begin(), scratch.begin() + static_cast<std::ptrdiff_t>(datagram.size))});
        } catch (...) {
            recordIoError();
            return any;
        }
    }
    return any;
}

}
