#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include "engine/foundation/result.hpp"

namespace engine::networking {

class ByteReader final {
public:
    explicit ByteReader(std::span<const std::byte> data) noexcept;

    [[nodiscard]] foundation::Result<std::uint8_t> readU8();
    [[nodiscard]] foundation::Result<std::uint16_t> readU16();
    [[nodiscard]] foundation::Result<std::uint32_t> readU32();
    [[nodiscard]] foundation::Status readBytes(std::span<std::byte> destination);
    [[nodiscard]] foundation::Status skip(std::size_t count);

    [[nodiscard]] bool takeU8(std::uint8_t& value) noexcept;
    [[nodiscard]] bool takeU16(std::uint16_t& value) noexcept;
    [[nodiscard]] bool takeU32(std::uint32_t& value) noexcept;
    [[nodiscard]] bool takeView(std::size_t count, std::span<const std::byte>& view) noexcept;

    [[nodiscard]] std::size_t position() const noexcept;
    [[nodiscard]] std::size_t remaining() const noexcept;
    [[nodiscard]] bool isFullyConsumed() const noexcept;
    [[nodiscard]] bool hasFailed() const noexcept;

private:
    [[nodiscard]] bool reserve(std::size_t count) noexcept;
    [[nodiscard]] foundation::Status claim(std::size_t count);
    [[nodiscard]] foundation::Result<std::uint32_t> readInteger(std::size_t width);

    std::span<const std::byte> data_;
    std::size_t position_ = 0;
    bool failed_ = false;
};

}
