#pragma once

#include <type_traits>
#include <utility>

namespace engine::events {

template <typename Event>
SubscriptionId EventBus::subscribe(std::function<void(const Event&)> handler) {
    return add(std::type_index(typeid(Event)),
               [handler = std::move(handler)](const void* event) { handler(*static_cast<const Event*>(event)); });
}

template <typename Event>
void EventBus::publish(const Event& event) {
    dispatch(std::type_index(typeid(Event)), &event);
}

template <typename Event>
foundation::Status EventBus::enqueue(Event event) {
    static_assert(std::is_copy_constructible_v<Event>, "un evenement en file doit etre copiable");
    return enqueueTask([this, event = std::move(event)]() { publish(event); });
}

template <typename Event>
std::size_t EventBus::subscriberCount() const {
    return countActive(std::type_index(typeid(Event)));
}

}
