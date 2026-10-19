#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace engine::networking {

inline constexpr std::size_t kAckBitCount = 32;
inline constexpr std::size_t kAckWindowSize = kAckBitCount + 1;

enum class ReceptionOutcome {
    Accepted,
    Duplicate,
    TooOld,
};

class AckWindow final {
public:
    [[nodiscard]] ReceptionOutcome record(std::uint16_t sequence) noexcept;

    [[nodiscard]] bool hasReceived() const noexcept;
    [[nodiscard]] std::uint16_t ack() const noexcept;
    [[nodiscard]] std::uint32_t ackBits() const noexcept;

private:
    void advanceTo(std::uint16_t sequence) noexcept;
    [[nodiscard]] ReceptionOutcome recordOlder(std::uint16_t sequence) noexcept;

    std::uint16_t ack_ = 0;
    std::uint32_t ackBits_ = 0;
    bool received_ = false;
};

[[nodiscard]] std::size_t decodeAcknowledged(std::uint16_t ack, std::uint32_t ackBits,
                                             std::span<std::uint16_t, kAckWindowSize> acknowledged) noexcept;

}
