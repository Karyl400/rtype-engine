#include "engine/ecs/world.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace engine::ecs {

World::ComponentStorageBase::~ComponentStorageBase() = default;

World::World() = default;

World::~World() = default;

Entity World::create() {
    if (nextEntityId_ > std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("identifiants d'entite epuises");
    }
    const auto id = static_cast<std::uint32_t>(nextEntityId_++);
    aliveEntities_.insert(id);
    return Entity{id};
}

foundation::Status World::destroy(Entity entity) {
    if (!alive(entity)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "entite inconnue");
    }
    for (auto& [type, store] : componentStores_) {
        static_cast<void>(type);
        store->erase(entity.value);
    }
    aliveEntities_.erase(entity.value);
    return foundation::okStatus();
}

bool World::alive(Entity entity) const {
    return aliveEntities_.contains(entity.value);
}

std::size_t World::entityCount() const {
    return aliveEntities_.size();
}

std::vector<Entity> World::entities() const {
    std::vector<Entity> alive;
    alive.reserve(entityCount());
    for (const std::uint32_t id : aliveEntities_) {
        alive.push_back(Entity{id});
    }
    std::sort(alive.begin(), alive.end());
    return alive;
}

void World::clear() {
    aliveEntities_.clear();
    componentStores_.clear();
}

}
