#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "engine/ecs/entity.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::ecs {

class World final : private foundation::NonCopyable {
public:
    World();
    ~World();

    [[nodiscard]] Entity create();
    [[nodiscard]] foundation::Status destroy(Entity entity);
    [[nodiscard]] bool alive(Entity entity) const;
    [[nodiscard]] std::size_t entityCount() const;
    [[nodiscard]] std::vector<Entity> entities() const;
    void clear();

    template <typename Component, typename... Args>
    [[nodiscard]] foundation::Status add(Entity entity, Args&&... args);

    template <typename Component>
    [[nodiscard]] foundation::Status remove(Entity entity);

    template <typename Component>
    [[nodiscard]] bool has(Entity entity) const;

    template <typename Component>
    [[nodiscard]] Component* get(Entity entity);

    template <typename Component>
    [[nodiscard]] const Component* get(Entity entity) const;

    template <typename... Components, typename Function>
    void each(Function&& function);

private:
    struct ComponentStorageBase {
        virtual ~ComponentStorageBase();
        virtual void erase(std::uint32_t entity) = 0;
        [[nodiscard]] virtual bool contains(std::uint32_t entity) const = 0;
    };

    template <typename Component>
    struct ComponentStorage;

    template <typename Component>
    [[nodiscard]] ComponentStorage<Component>& storage();

    template <typename Component>
    [[nodiscard]] ComponentStorage<Component>* findStorage();

    template <typename Component>
    [[nodiscard]] const ComponentStorage<Component>* findStorage() const;

    std::unordered_set<std::uint32_t> aliveEntities_;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentStorageBase>> componentStores_;
    std::uint64_t nextEntityId_ = 0;
};

}

#include "engine/ecs/world.tpp"
