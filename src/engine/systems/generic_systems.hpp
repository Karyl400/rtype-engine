#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include "engine/ecs/world.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::systems {

inline constexpr std::size_t kMaxPendingSpawns = 1024;

using Prefab = std::function<void(ecs::World&, ecs::Entity)>;

class Spawner final : private foundation::NonCopyable {
public:
    [[nodiscard]] foundation::Status request(Prefab prefab);
    std::size_t flush(ecs::World& world);
    [[nodiscard]] std::size_t pending() const noexcept;
    [[nodiscard]] std::uint64_t faultCount() const noexcept;

private:
    std::vector<Prefab> queue_;
    std::uint64_t faults_ = 0;
};

void movementSystem(ecs::World& world, std::chrono::nanoseconds step);
void lifetimeSystem(ecs::World& world, std::chrono::nanoseconds step);
std::size_t destructionSystem(ecs::World& world);

}
