#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace engine::foundation {

template <typename Tag, typename Rep = std::uint32_t>
struct StrongId {
    Rep value{};

    friend auto operator<=>(const StrongId&, const StrongId&) = default;
};

}

namespace std {

template <typename Tag, typename Rep>
struct hash<engine::foundation::StrongId<Tag, Rep>> {
    [[nodiscard]] std::size_t operator()(const engine::foundation::StrongId<Tag, Rep>& id) const noexcept;
};

}

#include "engine/foundation/strong_id.tpp"
