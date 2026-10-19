#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace engine::foundation {

enum class ErrorCode {
    InvalidArgument,
    NotFound,
    Io,
    Unsupported,
    Timeout,
    Closed,
    PermissionDenied,
};

[[nodiscard]] std::string_view errorCodeName(ErrorCode code);

struct Error {
    ErrorCode code = ErrorCode::InvalidArgument;
    std::string message;
};

struct Unit {};

template <typename T, typename E = Error>
class [[nodiscard]] Result {
public:
    [[nodiscard]] static Result ok(T value);
    [[nodiscard]] static Result fail(E error);

    [[nodiscard]] bool hasValue() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] const T& value() const&;
    [[nodiscard]] T& value() &;
    [[nodiscard]] T&& value() &&;
    [[nodiscard]] const E& error() const&;

    [[nodiscard]] T valueOr(T fallback) const&;

private:
    template <std::size_t I, typename... Args>
    Result(std::in_place_index_t<I> index, Args&&... args);

    std::variant<T, E> data_;
};

using Status = Result<Unit>;

[[nodiscard]] Status okStatus();
[[nodiscard]] Status failStatus(ErrorCode code, std::string message);

}

#include "engine/foundation/result.tpp"
