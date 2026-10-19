#pragma once

#include <utility>

namespace engine::runtime {

template <typename Component>
foundation::Status SnapshotRecorder::track(const std::string& name, std::function<std::string(const Component&)> describe) {
    if (!describe) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "description vide");
    }
    return addTracker(name, [describe = std::move(describe)](const ecs::World& world, ecs::Entity entity, std::string& out) {
        const Component* component = world.get<Component>(entity);
        if (component == nullptr) {
            return false;
        }
        out = describe(*component);
        return true;
    });
}

}
