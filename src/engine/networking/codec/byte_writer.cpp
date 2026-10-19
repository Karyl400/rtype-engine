#include "engine/networking/codec/byte_writer.hpp"

#include <algorithm>

#include "engine/networking/codec/byte_order.hpp"

namespace engine::networking {

namespace {

constexpr const char* kOutOfBoundsMessage = "hors limites";

}

ByteWriter::ByteWriter(std::span<std::byte> buffer) noexcept : buffer_(buffer) {}

foundation::Status ByteWriter::writeU8(std::uint8_t value) {
    return writeInteger(value, sizeof(std::uint8_t));
}

foundation::Status ByteWriter::writeU16(std::uint16_t value) {
    return writeInteger(value, sizeof(std::uint16_t));
}

foundation::Status ByteWriter::writeU32(std::uint32_t value) {
    return writeInteger(value, sizeof(std::uint32_t));
}

foundation::Status ByteWriter::writeBytes(std::span<const std::byte> bytes) {
    if (remaining() < bytes.size()) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, kOutOfBoundsMessage);
    }
    std::copy(bytes.begin(), bytes.end(), buffer_.subspan(position_).begin());
    position_ += bytes.size();
    return foundation::okStatus();
}

std::size_t ByteWriter::size() const noexcept {
    return position_;
}

std::size_t ByteWriter::remaining() const noexcept {
    return buffer_.size() - position_;
}

foundation::Status ByteWriter::writeInteger(std::uint32_t value, std::size_t width) {
    if (remaining() < width) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, kOutOfBoundsMessage);
    }
    foundation::Status stored = storeBigEndian(buffer_.subspan(position_, width), value);
    if (!stored.hasValue()) {
        return stored;
    }
    position_ += width;
    return foundation::okStatus();
}

}
