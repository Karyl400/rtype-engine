#pragma once

#include <any>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "engine/commands/client_id.hpp"
#include "engine/ecs/world.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::commands {

inline constexpr std::size_t kMaxPendingPerClient = 64;
inline constexpr std::size_t kMaxPendingTotal = 4096;

struct ProcessReport {
    std::uint32_t executed = 0;
    std::uint32_t rejected = 0;
    std::uint32_t faulted = 0;
};

class CommandProcessor final : private foundation::NonCopyable {
public:
    CommandProcessor();
    ~CommandProcessor();

    template <typename Command, typename Validator, typename Executor>
    [[nodiscard]] foundation::Status registerCommand(Validator validator, Executor executor);

    template <typename Command>
    [[nodiscard]] foundation::Status submit(ClientId client, Command command);

    [[nodiscard]] ProcessReport process(ecs::World& world);
    [[nodiscard]] std::size_t dropClient(ClientId client);

    [[nodiscard]] std::size_t pending() const noexcept;
    [[nodiscard]] std::size_t pending(ClientId client) const;

private:
    using Handler = std::function<foundation::Status(ecs::World&, ClientId, const std::any&)>;

    struct Entry {
        ClientId client;
        std::type_index type;
        std::any payload;
    };

    [[nodiscard]] foundation::Status addHandler(std::type_index type, Handler handler);
    [[nodiscard]] foundation::Status enqueue(ClientId client, std::type_index type, std::any payload);
    void run(Entry& entry, ecs::World& world, ProcessReport& report);

    std::unordered_map<std::type_index, Handler> handlers_;
    std::vector<Entry> queue_;
    std::unordered_map<ClientId, std::size_t> perClient_;
};

}

#include "engine/commands/command_processor.tpp"
