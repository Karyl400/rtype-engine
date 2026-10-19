#pragma once

#include <functional>
#include <typeinfo>

namespace engine::ecs {

template <typename Component>
struct World::ComponentStorage final : ComponentStorageBase {
    std::unordered_map<std::uint32_t, Component> components;

    void erase(std::uint32_t entity) override {
        components.erase(entity);
    }

    [[nodiscard]] bool contains(std::uint32_t entity) const override {
        return components.contains(entity);
    }
};

template <typename Component>
typename World::template ComponentStorage<Component>& World::storage() {
    const std::type_index type(typeid(Component));
    auto found = componentStores_.find(type);
    if (found == componentStores_.end()) {
        auto created = std::make_unique<ComponentStorage<Component>>();
        auto* result = created.get();
        componentStores_.emplace(type, std::move(created));
        return *result;
    }
    return *static_cast<ComponentStorage<Component>*>(found->second.get());
}

template <typename Component>
typename World::template ComponentStorage<Component>* World::findStorage() {
    const auto found = componentStores_.find(std::type_index(typeid(Component)));
    if (found == componentStores_.end()) return nullptr;
    return static_cast<ComponentStorage<Component>*>(found->second.get());
}

template <typename Component>
const typename World::template ComponentStorage<Component>* World::findStorage() const {
    const auto found = componentStores_.find(std::type_index(typeid(Component)));
    if (found == componentStores_.end()) return nullptr;
    return static_cast<const ComponentStorage<Component>*>(found->second.get());
}

template <typename Component, typename... Args>
foundation::Status World::add(Entity entity, Args&&... args) {
    if (!alive(entity)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "entite inconnue");
    }
    auto& components = storage<Component>().components;
    if (components.contains(entity.value)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "composant deja present");
    }
    components.try_emplace(entity.value, std::forward<Args>(args)...);
    return foundation::okStatus();
}

template <typename Component>
foundation::Status World::remove(Entity entity) {
    auto* store = findStorage<Component>();
    if (!alive(entity) || store == nullptr || !store->contains(entity.value)) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "composant absent");
    }
    store->erase(entity.value);
    return foundation::okStatus();
}

template <typename Component>
bool World::has(Entity entity) const {
    const auto* store = findStorage<Component>();
    return alive(entity) && store != nullptr && store->contains(entity.value);
}

template <typename Component>
Component* World::get(Entity entity) {
    auto* store = findStorage<Component>();
    if (!alive(entity) || store == nullptr) return nullptr;
    const auto found = store->components.find(entity.value);
    return found == store->components.end() ? nullptr : &found->second;
}

template <typename Component>
const Component* World::get(Entity entity) const {
    const auto* store = findStorage<Component>();
    if (!alive(entity) || store == nullptr) return nullptr;
    const auto found = store->components.find(entity.value);
    return found == store->components.end() ? nullptr : &found->second;
}

template <typename... Components, typename Function>
void World::each(Function&& function) {
    for (const std::uint32_t id : aliveEntities_) {
        const Entity entity{id};
        if ((has<Components>(entity) && ...)) {
            std::invoke(function, entity, *get<Components>(entity)...);
        }
    }
}

}
