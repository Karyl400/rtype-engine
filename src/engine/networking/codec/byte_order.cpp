#include "engine/networking/codec/byte_order.hpp"

namespace engine::networking {

namespace {

constexpr std::size_t kBitsPerByte = 8;
constexpr std::uint32_t kByteMask = 0xFF;
constexpr std::size_t kMaxIntegerWidth = sizeof(std::uint32_t);
constexpr const char* kInvalidWidthMessage = "taille invalide";

bool isSupportedWidth(std::size_t width) noexcept {
    return width > 0 && width <= kMaxIntegerWidth;
}

void spread(std::span<std::byte> destination, std::uint32_t value) noexcept {
    const std::size_t width = destination.size();
    for (std::size_t index = 0; index < width; ++index) {
        const std::size_t shift = (width - index - 1) * kBitsPerByte;
        destination[index] = static_cast<std::byte>((value >> shift) & kByteMask);
    }
}

std::uint32_t accumulate(std::span<const std::byte> source) noexcept {
    std::uint32_t result = 0;
    for (const std::byte current : source) {
        result = (result << kBitsPerByte) | std::to_integer<std::uint32_t>(current);
    }
    return result;
}

}

foundation::Status storeBigEndian(std::span<std::byte> destination, std::uint32_t value) {
    if (!isSupportedWidth(destination.size())) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, kInvalidWidthMessage);
    }
    spread(destination, value);
    return foundation::okStatus();
}

foundation::Result<std::uint32_t> loadBigEndian(std::span<const std::byte> source) {
    if (!isSupportedWidth(source.size())) {
        return foundation::Result<std::uint32_t>::fail(foundation::Error{foundation::ErrorCode::InvalidArgument, kInvalidWidthMessage});
    }
    return foundation::Result<std::uint32_t>::ok(accumulate(source));
}

void storeU8(std::span<std::byte, sizeof(std::uint8_t)> destination, std::uint8_t value) noexcept {
    spread(destination, value);
}

void storeU16(std::span<std::byte, sizeof(std::uint16_t)> destination, std::uint16_t value) noexcept {
    spread(destination, value);
}

void storeU32(std::span<std::byte, sizeof(std::uint32_t)> destination, std::uint32_t value) noexcept {
    spread(destination, value);
}

std::uint8_t loadU8(std::span<const std::byte, sizeof(std::uint8_t)> source) noexcept {
    return static_cast<std::uint8_t>(accumulate(source));
}

std::uint16_t loadU16(std::span<const std::byte, sizeof(std::uint16_t)> source) noexcept {
    return static_cast<std::uint16_t>(accumulate(source));
}

std::uint32_t loadU32(std::span<const std::byte, sizeof(std::uint32_t)> source) noexcept {
    return accumulate(source);
}

}
