#include "engine/networking/codec/byte_reader.hpp"

#include <algorithm>

#include "engine/networking/codec/byte_order.hpp"

namespace engine::networking {

namespace {

constexpr const char* kOutOfBoundsMessage = "hors limites";

template <typename Narrow>
foundation::Result<Narrow> narrow(const foundation::Result<std::uint32_t>& wide) {
    if (!wide.hasValue()) {
        return foundation::Result<Narrow>::fail(wide.error());
    }
    return foundation::Result<Narrow>::ok(static_cast<Narrow>(wide.value()));
}

}

ByteReader::ByteReader(std::span<const std::byte> data) noexcept : data_(data) {}

foundation::Result<std::uint8_t> ByteReader::readU8() {
    return narrow<std::uint8_t>(readInteger(sizeof(std::uint8_t)));
}

foundation::Result<std::uint16_t> ByteReader::readU16() {
    return narrow<std::uint16_t>(readInteger(sizeof(std::uint16_t)));
}

foundation::Result<std::uint32_t> ByteReader::readU32() {
    return readInteger(sizeof(std::uint32_t));
}

foundation::Status ByteReader::readBytes(std::span<std::byte> destination) {
    foundation::Status claimed = claim(destination.size());
    if (!claimed.hasValue()) {
        return claimed;
    }
    const auto source = data_.subspan(position_, destination.size());
    std::copy(source.begin(), source.end(), destination.begin());
    position_ += destination.size();
    return foundation::okStatus();
}

foundation::Status ByteReader::skip(std::size_t count) {
    foundation::Status claimed = claim(count);
    if (!claimed.hasValue()) {
        return claimed;
    }
    position_ += count;
    return foundation::okStatus();
}

std::size_t ByteReader::position() const noexcept {
    return position_;
}

std::size_t ByteReader::remaining() const noexcept {
    return data_.size() - position_;
}

bool ByteReader::isFullyConsumed() const noexcept {
    return !failed_ && remaining() == 0;
}

bool ByteReader::hasFailed() const noexcept {
    return failed_;
}

bool ByteReader::takeU8(std::uint8_t& value) noexcept {
    std::span<const std::byte> view;
    if (!takeView(sizeof(std::uint8_t), view)) {
        return false;
    }
    value = loadU8(view.first<sizeof(std::uint8_t)>());
    return true;
}

bool ByteReader::takeU16(std::uint16_t& value) noexcept {
    std::span<const std::byte> view;
    if (!takeView(sizeof(std::uint16_t), view)) {
        return false;
    }
    value = loadU16(view.first<sizeof(std::uint16_t)>());
    return true;
}

bool ByteReader::takeU32(std::uint32_t& value) noexcept {
    std::span<const std::byte> view;
    if (!takeView(sizeof(std::uint32_t), view)) {
        return false;
    }
    value = loadU32(view.first<sizeof(std::uint32_t)>());
    return true;
}

bool ByteReader::takeView(std::size_t count, std::span<const std::byte>& view) noexcept {
    if (!reserve(count)) {
        return false;
    }
    view = data_.subspan(position_, count);
    position_ += count;
    return true;
}

bool ByteReader::reserve(std::size_t count) noexcept {
    if (failed_ || remaining() < count) {
        failed_ = true;
        return false;
    }
    return true;
}

foundation::Status ByteReader::claim(std::size_t count) {
    if (!reserve(count)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, kOutOfBoundsMessage);
    }
    return foundation::okStatus();
}

foundation::Result<std::uint32_t> ByteReader::readInteger(std::size_t width) {
    foundation::Status claimed = claim(width);
    if (!claimed.hasValue()) {
        return foundation::Result<std::uint32_t>::fail(claimed.error());
    }
    foundation::Result<std::uint32_t> loaded = loadBigEndian(data_.subspan(position_, width));
    if (!loaded.hasValue()) {
        failed_ = true;
        return loaded;
    }
    position_ += width;
    return loaded;
}

}
