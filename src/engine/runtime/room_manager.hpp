#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "engine/commands/client_id.hpp"
#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"
#include "engine/runtime/room.hpp"
#include "engine/time/api/fixed_timestep.hpp"
#include "engine/time/api/iclock.hpp"

namespace engine::runtime {

inline constexpr std::size_t kMaxRooms = 16;
inline constexpr std::uint32_t kMaxConsecutiveRoomFaults = 3;

using RoomSetup = std::function<foundation::Status(Room&)>;

struct RoomsReport {
    std::uint32_t ticked = 0;
    std::uint32_t faulted = 0;
    std::uint32_t skipped = 0;
};

class RoomManager final : private foundation::NonCopyable {
public:
    RoomManager();
    ~RoomManager();

    [[nodiscard]] foundation::Result<RoomId> create(time::FixedTimestep timestep, const RoomSetup& setup = nullptr);
    [[nodiscard]] foundation::Status close(RoomId id);
    [[nodiscard]] Room* find(RoomId id);
    [[nodiscard]] const Room* find(RoomId id) const;

    [[nodiscard]] foundation::Status join(RoomId id, commands::ClientId client);
    [[nodiscard]] foundation::Status leave(commands::ClientId client);
    [[nodiscard]] std::optional<RoomId> roomOf(commands::ClientId client) const;

    [[nodiscard]] RoomsReport tickAll(const time::IClock& clock);

    [[nodiscard]] std::size_t roomCount() const noexcept;
    [[nodiscard]] std::size_t quarantinedCount() const noexcept;
    [[nodiscard]] bool quarantined(RoomId id) const;
    [[nodiscard]] std::vector<RoomId> ids() const;

private:
    struct Entry {
        std::unique_ptr<Room> room;
        std::uint32_t consecutiveFaults = 0;
        bool quarantined = false;
    };

    std::map<RoomId, Entry> rooms_;
    std::unordered_map<commands::ClientId, RoomId> clientRoom_;
    std::uint32_t nextId_ = 1;
};

}
