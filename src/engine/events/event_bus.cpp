#include "engine/events/event_bus.hpp"

#include <algorithm>
#include <utility>

namespace engine::events {

EventBus::EventBus() = default;

EventBus::~EventBus() = default;

SubscriptionId EventBus::add(std::type_index type, std::function<void(const void*)> handler) {
    const SubscriptionId id{nextId_++};
    slots_[type].push_back(Slot{id, std::move(handler), true});
    owners_.emplace(id.value, type);
    return id;
}

foundation::Status EventBus::unsubscribe(SubscriptionId id) {
    const auto owner = owners_.find(id.value);
    if (owner == owners_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "abonnement inconnu");
    }
    for (Slot& slot : slots_[owner->second]) {
        if (slot.id == id) {
            slot.active = false;
        }
    }
    owners_.erase(owner);
    dirty_ = true;
    if (depth_ == 0) {
        compact();
    }
    return foundation::okStatus();
}

void EventBus::compact() {
    for (auto& entry : slots_) {
        std::erase_if(entry.second, [](const Slot& slot) { return !slot.active; });
    }
    dirty_ = false;
}

void EventBus::invoke(Slot& slot, const void* event) {
    try {
        slot.handler(event);
    } catch (...) {
        ++faults_;
    }
}

void EventBus::dispatch(std::type_index type, const void* event) {
    const auto found = slots_.find(type);
    if (found == slots_.end()) {
        return;
    }
    const std::size_t count = found->second.size();
    ++depth_;
    for (std::size_t i = 0; i < count; ++i) {
        std::vector<Slot>& list = slots_[type];
        if (list[i].active) {
            Slot copy = list[i];
            invoke(copy, event);
        }
    }
    --depth_;
    if (depth_ == 0 && dirty_) {
        compact();
    }
}

foundation::Status EventBus::enqueueTask(std::function<void()> task) {
    if (queue_.size() >= kMaxQueuedEvents) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "file d'evenements pleine");
    }
    queue_.push_back(std::move(task));
    return foundation::okStatus();
}

std::size_t EventBus::flush() {
    std::vector<std::function<void()>> batch;
    batch.swap(queue_);
    for (const std::function<void()>& task : batch) {
        task();
    }
    return batch.size();
}

std::size_t EventBus::countActive(std::type_index type) const {
    const auto found = slots_.find(type);
    if (found == slots_.end()) {
        return 0;
    }
    return static_cast<std::size_t>(
        std::count_if(found->second.begin(), found->second.end(), [](const Slot& slot) { return slot.active; }));
}

std::size_t EventBus::pending() const noexcept {
    return queue_.size();
}

std::uint64_t EventBus::faultCount() const noexcept {
    return faults_;
}

}
