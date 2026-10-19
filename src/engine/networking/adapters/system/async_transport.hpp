#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include "engine/networking/api/inetwork_transport.hpp"

namespace engine::networking {

inline constexpr std::size_t kAsyncQueueCapacity = 1024;
inline constexpr std::size_t kAsyncMaxDatagram = 2048;
inline constexpr std::size_t kAsyncMaxPumpPerPass = 256;

class AsyncTransport final : public INetworkTransport {
public:
    explicit AsyncTransport(INetworkTransport& inner);
    ~AsyncTransport() override;

    [[nodiscard]] foundation::Status send(const Endpoint& to, std::span<const std::byte> payload) override;
    [[nodiscard]] foundation::Result<std::optional<ReceivedDatagram>> poll(std::span<std::byte> buffer) override;

    [[nodiscard]] std::uint64_t droppedInbound() const noexcept;
    [[nodiscard]] std::uint64_t ioErrors() const noexcept;
    [[nodiscard]] std::uint64_t forwarded() const noexcept;

private:
    struct Packet {
        Endpoint peer;
        std::vector<std::byte> payload;
    };

    void ioLoop();
    [[nodiscard]] bool flushOutbound();
    [[nodiscard]] bool pumpInbound();
    void recordIoError();

    INetworkTransport& inner_;
    std::mutex mutex_;
    std::condition_variable wake_;
    std::deque<Packet> inbox_;
    std::deque<Packet> outbox_;
    std::uint64_t pendingErrors_ = 0;
    std::atomic<std::uint64_t> dropped_{0};
    std::atomic<std::uint64_t> ioErrors_{0};
    std::atomic<std::uint64_t> forwarded_{0};
    std::atomic<bool> stopping_{false};
    std::thread thread_;
};

}
