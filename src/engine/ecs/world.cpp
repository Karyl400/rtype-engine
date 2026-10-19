#include "engine/ecs/world.hpp"

#include <algorithm>

namespace engine::ecs {

namespace detail {

entt::entity toNative(Entity entity) {
    return static_cast<entt::entity>(entity.value);
}

Entity fromNative(entt::entity native) {
    return Entity{static_cast<std::uint32_t>(native)};
}

}

World::World() = default;

World::~World() = default;

Entity World::create() {
    return detail::fromNative(registry_.create());
}

foundation::Status World::destroy(Entity entity) {
    const auto native = detail::toNative(entity);
    if (!registry_.valid(native)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "entite inconnue");
    }
    registry_.destroy(native);
    return foundation::okStatus();
}

bool World::alive(Entity entity) const {
    return registry_.valid(detail::toNative(entity));
}

std::size_t World::entityCount() const {
    return registry_.storage<entt::entity>()->free_list();
}

std::vector<Entity> World::entities() const {
    std::vector<Entity> alive;
    alive.reserve(entityCount());
    for (const entt::entity native : registry_.view<entt::entity>()) {
        alive.push_back(detail::fromNative(native));
    }
    std::sort(alive.begin(), alive.end());
    return alive;
}

void World::clear() {
    registry_.clear();
}

}
