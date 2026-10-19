#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "engine/foundation/result.hpp"

namespace engine::networking {

[[nodiscard]] foundation::Status storeBigEndian(std::span<std::byte> destination, std::uint32_t value);
[[nodiscard]] foundation::Result<std::uint32_t> loadBigEndian(std::span<const std::byte> source);

void storeU8(std::span<std::byte, sizeof(std::uint8_t)> destination, std::uint8_t value) noexcept;
void storeU16(std::span<std::byte, sizeof(std::uint16_t)> destination, std::uint16_t value) noexcept;
void storeU32(std::span<std::byte, sizeof(std::uint32_t)> destination, std::uint32_t value) noexcept;

[[nodiscard]] std::uint8_t loadU8(std::span<const std::byte, sizeof(std::uint8_t)> source) noexcept;
[[nodiscard]] std::uint16_t loadU16(std::span<const std::byte, sizeof(std::uint16_t)> source) noexcept;
[[nodiscard]] std::uint32_t loadU32(std::span<const std::byte, sizeof(std::uint32_t)> source) noexcept;

}
