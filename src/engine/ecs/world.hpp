#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include <entt/entity/registry.hpp>

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
    entt::registry registry_;
};

}

#include "engine/ecs/world.tpp"
