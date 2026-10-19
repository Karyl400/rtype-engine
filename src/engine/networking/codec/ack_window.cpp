#include "engine/networking/codec/ack_window.hpp"

#include "engine/networking/codec/sequence_number.hpp"

namespace engine::networking {

namespace {

constexpr std::uint32_t kLowestBit = 1U;
constexpr std::uint32_t kNoBits = 0U;

std::uint32_t bitForDistance(std::uint16_t distance) noexcept {
    return kLowestBit << (distance - 1U);
}

}

ReceptionOutcome AckWindow::record(std::uint16_t sequence) noexcept {
    if (!received_) {
        received_ = true;
        ack_ = sequence;
        ackBits_ = kNoBits;
        return ReceptionOutcome::Accepted;
    }
    if (sequence == ack_) {
        return ReceptionOutcome::Duplicate;
    }
    if (isSequenceMoreRecent(sequence, ack_)) {
        advanceTo(sequence);
        return ReceptionOutcome::Accepted;
    }
    return recordOlder(sequence);
}

bool AckWindow::hasReceived() const noexcept {
    return received_;
}

std::uint16_t AckWindow::ack() const noexcept {
    return ack_;
}

std::uint32_t AckWindow::ackBits() const noexcept {
    return ackBits_;
}

void AckWindow::advanceTo(std::uint16_t sequence) noexcept {
    const std::uint16_t distance = sequenceDistance(sequence, ack_);
    if (distance < kAckBitCount) {
        ackBits_ = (ackBits_ << distance) | bitForDistance(distance);
    } else if (distance == kAckBitCount) {
        ackBits_ = bitForDistance(distance);
    } else {
        ackBits_ = kNoBits;
    }
    ack_ = sequence;
}

ReceptionOutcome AckWindow::recordOlder(std::uint16_t sequence) noexcept {
    const std::uint16_t distance = sequenceDistance(ack_, sequence);
    if (distance > kAckBitCount) {
        return ReceptionOutcome::TooOld;
    }
    const std::uint32_t mask = bitForDistance(distance);
    if ((ackBits_ & mask) != kNoBits) {
        return ReceptionOutcome::Duplicate;
    }
    ackBits_ |= mask;
    return ReceptionOutcome::Accepted;
}

std::size_t decodeAcknowledged(std::uint16_t ack, std::uint32_t ackBits, std::span<std::uint16_t, kAckWindowSize> acknowledged) noexcept {
    std::size_t count = 0;
    acknowledged[count] = ack;
    ++count;
    for (std::uint16_t distance = 1; distance <= kAckBitCount; ++distance) {
        if ((ackBits & bitForDistance(distance)) != kNoBits) {
            acknowledged[count] = static_cast<std::uint16_t>(ack - distance);
            ++count;
        }
    }
    return count;
}

}
