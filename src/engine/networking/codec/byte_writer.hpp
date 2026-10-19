#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "engine/foundation/result.hpp"

namespace engine::networking {

class ByteWriter final {
public:
    explicit ByteWriter(std::span<std::byte> buffer) noexcept;

    [[nodiscard]] foundation::Status writeU8(std::uint8_t value);
    [[nodiscard]] foundation::Status writeU16(std::uint16_t value);
    [[nodiscard]] foundation::Status writeU32(std::uint32_t value);
    [[nodiscard]] foundation::Status writeBytes(std::span<const std::byte> bytes);

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t remaining() const noexcept;

private:
    [[nodiscard]] foundation::Status writeInteger(std::uint32_t value, std::size_t width);

    std::span<std::byte> buffer_;
    std::size_t position_ = 0;
};

}
