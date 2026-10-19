#pragma once

namespace engine::foundation {

template <typename T, typename E>
Result<T, E> Result<T, E>::ok(T value) {
    return Result{std::in_place_index<0>, std::move(value)};
}

template <typename T, typename E>
Result<T, E> Result<T, E>::fail(E error) {
    return Result{std::in_place_index<1>, std::move(error)};
}

template <typename T, typename E>
bool Result<T, E>::hasValue() const noexcept {
    return data_.index() == 0;
}

template <typename T, typename E>
Result<T, E>::operator bool() const noexcept {
    return hasValue();
}

template <typename T, typename E>
const T& Result<T, E>::value() const& {
    return std::get<0>(data_);
}

template <typename T, typename E>
T& Result<T, E>::value() & {
    return std::get<0>(data_);
}

template <typename T, typename E>
T&& Result<T, E>::value() && {
    return std::get<0>(std::move(data_));
}

template <typename T, typename E>
const E& Result<T, E>::error() const& {
    return std::get<1>(data_);
}

template <typename T, typename E>
T Result<T, E>::valueOr(T fallback) const& {
    return hasValue() ? value() : std::move(fallback);
}

template <typename T, typename E>
template <std::size_t I, typename... Args>
Result<T, E>::Result(std::in_place_index_t<I> index, Args&&... args) : data_(index, std::forward<Args>(args)...) {}

}
