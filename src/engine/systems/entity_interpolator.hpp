#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

#include "engine/ecs/entity.hpp"
#include "engine/ecs/world.hpp"
#include "engine/foundation/vec2.hpp"

namespace engine::systems {

struct EntityState {
    ecs::Entity entity{};
    foundation::Vec2 position{};
    foundation::Vec2 velocity{};
};

struct WorldSnapshot {
    std::uint64_t tick = 0;
    std::chrono::microseconds timestamp{0};
    std::vector<EntityState> entities{};
};

class EntityInterpolator {
public:
    EntityInterpolator();
    explicit EntityInterpolator(std::chrono::microseconds interpolationDelay);

    void setInterpolationDelay(std::chrono::microseconds delay);
    [[nodiscard]] std::chrono::microseconds interpolationDelay() const;

    void pushSnapshot(WorldSnapshot snapshot);
    void clear();

    [[nodiscard]] std::size_t snapshotCount() const;
    [[nodiscard]] std::optional<foundation::Vec2> samplePosition(ecs::Entity entity, std::chrono::microseconds renderTime) const;

    void update(ecs::World& world, std::chrono::microseconds renderTime);

    [[nodiscard]] float estimatedJitterMs() const;

private:
    std::chrono::microseconds interpolationDelay_{50000};
    std::deque<WorldSnapshot> snapshots_{};
    std::chrono::microseconds lastArrivalTime_{0};
    float estimatedJitterMs_{0.0F};
};

}
