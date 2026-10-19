#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/foundation/strong_id.hpp"

namespace engine::events {

inline constexpr std::size_t kMaxQueuedEvents = 4096;

struct SubscriptionTag {};

using SubscriptionId = foundation::StrongId<SubscriptionTag, std::uint32_t>;

class EventBus final : private foundation::NonCopyable {
public:
    EventBus();
    ~EventBus();

    template <typename Event>
    [[nodiscard]] SubscriptionId subscribe(std::function<void(const Event&)> handler);

    [[nodiscard]] foundation::Status unsubscribe(SubscriptionId id);

    template <typename Event>
    void publish(const Event& event);

    template <typename Event>
    [[nodiscard]] foundation::Status enqueue(Event event);

    [[nodiscard]] std::size_t flush();

    template <typename Event>
    [[nodiscard]] std::size_t subscriberCount() const;

    [[nodiscard]] std::size_t pending() const noexcept;
    [[nodiscard]] std::uint64_t faultCount() const noexcept;

private:
    struct Slot {
        SubscriptionId id;
        std::function<void(const void*)> handler;
        bool active = true;
    };

    [[nodiscard]] SubscriptionId add(std::type_index type, std::function<void(const void*)> handler);
    void dispatch(std::type_index type, const void* event);
    void invoke(Slot& slot, const void* event);
    void compact();
    [[nodiscard]] std::size_t countActive(std::type_index type) const;
    [[nodiscard]] foundation::Status enqueueTask(std::function<void()> task);

    std::unordered_map<std::type_index, std::vector<Slot>> slots_;
    std::unordered_map<std::uint32_t, std::type_index> owners_;
    std::vector<std::function<void()>> queue_;
    std::uint32_t nextId_ = 1;
    std::uint32_t depth_ = 0;
    bool dirty_ = false;
    std::uint64_t faults_ = 0;
};

}

#include "engine/events/event_bus.tpp"
