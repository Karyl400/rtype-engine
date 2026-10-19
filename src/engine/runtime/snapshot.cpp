#include "engine/runtime/snapshot.hpp"

#include <algorithm>
#include <utility>

namespace engine::runtime {

const EntityRecord* Snapshot::find(ecs::Entity entity) const {
    const auto found =
        std::find_if(entities.begin(), entities.end(), [entity](const EntityRecord& record) { return record.entity == entity; });
    return found == entities.end() ? nullptr : &*found;
}

const ComponentRecord* Snapshot::component(ecs::Entity entity, const std::string& name) const {
    const EntityRecord* record = find(entity);
    if (record == nullptr) {
        return nullptr;
    }
    const auto found = std::find_if(record->components.begin(), record->components.end(),
                                    [&name](const ComponentRecord& item) { return item.name == name; });
    return found == record->components.end() ? nullptr : &*found;
}

foundation::Status SnapshotRecorder::addTracker(std::string name, Reader read) {
    const bool duplicate =
        std::any_of(trackers_.begin(), trackers_.end(), [&name](const Tracker& tracker) { return tracker.name == name; });
    if (name.empty() || duplicate) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "nom de composant vide ou deja suivi");
    }
    trackers_.push_back(Tracker{std::move(name), std::move(read)});
    return foundation::okStatus();
}

Snapshot SnapshotRecorder::capture(const ecs::World& world, std::uint64_t step) const {
    Snapshot snapshot;
    snapshot.step = step;
    for (const ecs::Entity entity : world.entities()) {
        EntityRecord record{entity, {}};
        for (const Tracker& tracker : trackers_) {
            std::string value;
            if (tracker.read(world, entity, value)) {
                record.components.push_back(ComponentRecord{tracker.name, std::move(value)});
            }
        }
        if (!record.components.empty()) {
            snapshot.entities.push_back(std::move(record));
        }
    }
    return snapshot;
}

std::size_t SnapshotRecorder::trackedCount() const noexcept {
    return trackers_.size();
}

std::string toText(const Snapshot& snapshot) {
    std::string text = "step=" + std::to_string(snapshot.step) + "\n";
    for (const EntityRecord& record : snapshot.entities) {
        text += "entity " + std::to_string(record.entity.value) + ":";
        for (const ComponentRecord& component : record.components) {
            text += " " + component.name + "=" + component.value;
        }
        text += "\n";
    }
    return text;
}

}
