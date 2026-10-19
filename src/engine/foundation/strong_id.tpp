#pragma once

namespace std {

template <typename Tag, typename Rep>
std::size_t hash<engine::foundation::StrongId<Tag, Rep>>::operator()(const engine::foundation::StrongId<Tag, Rep>& id) const noexcept {
    return std::hash<Rep>{}(id.value);
}

}
