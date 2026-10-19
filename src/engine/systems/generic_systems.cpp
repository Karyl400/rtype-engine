#include "engine/systems/generic_systems.hpp"

#include <chrono>
#include <exception>
#include <utility>

#include "engine/systems/components.hpp"

namespace engine::systems {

foundation::Status Spawner::request(Prefab prefab) {
    if (!prefab) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "prefab vide");
    }
    if (queue_.size() >= kMaxPendingSpawns) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "file de creations pleine");
    }
    queue_.push_back(std::move(prefab));
    return foundation::okStatus();
}

std::size_t Spawner::flush(ecs::World& world) {
    std::vector<Prefab> batch;
    batch.swap(queue_);
    std::size_t created = 0;
    for (const Prefab& prefab : batch) {
        const ecs::Entity entity = world.create();
        try {
            prefab(world, entity);
            ++created;
        } catch (...) {
            ++faults_;
            (void)world.destroy(entity);
        }
    }
    return created;
}

std::size_t Spawner::pending() const noexcept {
    return queue_.size();
}

std::uint64_t Spawner::faultCount() const noexcept {
    return faults_;
}

void movementSystem(ecs::World& world, std::chrono::nanoseconds step) {
    const float seconds = std::chrono::duration<float>(step).count();
    world.each<Position, Velocity>(
        [seconds](ecs::Entity, Position& position, Velocity& velocity) { position.value = position.value + velocity.value * seconds; });
}

void lifetimeSystem(ecs::World& world, std::chrono::nanoseconds step) {
    std::vector<ecs::Entity> expired;
    world.each<Lifetime>([&expired, step](ecs::Entity entity, Lifetime& lifetime) {
        lifetime.remaining -= step;
        if (lifetime.remaining <= std::chrono::nanoseconds::zero()) {
            expired.push_back(entity);
        }
    });
    for (const ecs::Entity entity : expired) {
        if (!world.has<Expired>(entity)) {
            (void)world.add<Expired>(entity);
        }
    }
}

std::size_t destructionSystem(ecs::World& world) {
    std::vector<ecs::Entity> doomed;
    for (const ecs::Entity entity : world.entities()) {
        if (world.has<Expired>(entity)) {
            doomed.push_back(entity);
        }
    }
    std::size_t destroyed = 0;
    for (const ecs::Entity entity : doomed) {
        if (world.destroy(entity).hasValue()) {
            ++destroyed;
        }
    }
    return destroyed;
}

}
