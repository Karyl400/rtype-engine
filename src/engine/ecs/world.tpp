#pragma once

#include <string>

namespace engine::ecs {

namespace detail {

[[nodiscard]] entt::entity toNative(Entity entity);
[[nodiscard]] Entity fromNative(entt::entity native);

}

template <typename Component, typename... Args>
foundation::Status World::add(Entity entity, Args&&... args) {
    const auto native = detail::toNative(entity);
    if (!registry_.valid(native)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "entite inconnue");
    }
    if (registry_.all_of<Component>(native)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "composant deja present");
    }
    registry_.emplace<Component>(native, std::forward<Args>(args)...);
    return foundation::okStatus();
}

template <typename Component>
foundation::Status World::remove(Entity entity) {
    const auto native = detail::toNative(entity);
    if (!registry_.valid(native) || !registry_.all_of<Component>(native)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "composant absent");
    }
    registry_.remove<Component>(native);
    return foundation::okStatus();
}

template <typename Component>
bool World::has(Entity entity) const {
    const auto native = detail::toNative(entity);
    return registry_.valid(native) && registry_.all_of<Component>(native);
}

template <typename Component>
Component* World::get(Entity entity) {
    const auto native = detail::toNative(entity);
    if (!registry_.valid(native)) {
        return nullptr;
    }
    return registry_.try_get<Component>(native);
}

template <typename Component>
const Component* World::get(Entity entity) const {
    const auto native = detail::toNative(entity);
    if (!registry_.valid(native)) {
        return nullptr;
    }
    return registry_.try_get<Component>(native);
}

template <typename... Components, typename Function>
void World::each(Function&& function) {
    auto view = registry_.view<Components...>();
    for (const auto native : view) {
        function(detail::fromNative(native), view.template get<Components>(native)...);
    }
}

}
