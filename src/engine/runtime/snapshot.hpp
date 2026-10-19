#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "engine/ecs/world.hpp"
#include "engine/foundation/result.hpp"

namespace engine::runtime {

struct ComponentRecord {
    std::string name;
    std::string value;
};

struct EntityRecord {
    ecs::Entity entity;
    std::vector<ComponentRecord> components;
};

struct Snapshot {
    std::uint64_t step = 0;
    std::vector<EntityRecord> entities;

    [[nodiscard]] const EntityRecord* find(ecs::Entity entity) const;
    [[nodiscard]] const ComponentRecord* component(ecs::Entity entity, const std::string& name) const;
};

class SnapshotRecorder final {
public:
    template <typename Component>
    [[nodiscard]] foundation::Status track(const std::string& name, std::function<std::string(const Component&)> describe);

    [[nodiscard]] Snapshot capture(const ecs::World& world, std::uint64_t step) const;
    [[nodiscard]] std::size_t trackedCount() const noexcept;

private:
    using Reader = std::function<bool(const ecs::World&, ecs::Entity, std::string&)>;

    struct Tracker {
        std::string name;
        Reader read;
    };

    [[nodiscard]] foundation::Status addTracker(std::string name, Reader read);

    std::vector<Tracker> trackers_;
};

[[nodiscard]] std::string toText(const Snapshot& snapshot);

}

#include "engine/runtime/snapshot.tpp"
