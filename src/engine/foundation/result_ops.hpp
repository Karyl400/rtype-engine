#pragma once

#include <functional>
#include <type_traits>

#include "engine/foundation/result.hpp"

namespace engine::foundation {

template <typename T, typename E, typename F>
    requires(!std::is_void_v<std::invoke_result_t<F, const T&>>)
[[nodiscard]] auto transform(const Result<T, E>& result, F&& function) -> Result<std::invoke_result_t<F, const T&>, E>;

template <typename T, typename E, typename F>
[[nodiscard]] auto andThen(const Result<T, E>& result, F&& function) -> std::invoke_result_t<F, const T&>;

}

#include "engine/foundation/result_ops.tpp"
