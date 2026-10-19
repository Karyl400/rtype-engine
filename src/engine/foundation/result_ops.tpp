#pragma once

namespace engine::foundation {

template <typename T, typename E, typename F>
    requires(!std::is_void_v<std::invoke_result_t<F, const T&>>)
auto transform(const Result<T, E>& result, F&& function) -> Result<std::invoke_result_t<F, const T&>, E> {
    using Output = Result<std::invoke_result_t<F, const T&>, E>;
    if (!result.hasValue()) {
        return Output::fail(result.error());
    }
    return Output::ok(std::invoke(std::forward<F>(function), result.value()));
}

template <typename T, typename E, typename F>
auto andThen(const Result<T, E>& result, F&& function) -> std::invoke_result_t<F, const T&> {
    using Output = std::invoke_result_t<F, const T&>;
    if (!result.hasValue()) {
        return Output::fail(result.error());
    }
    return std::invoke(std::forward<F>(function), result.value());
}

}
