#include "engine/runtime/room_manager.hpp"

#include <utility>

namespace engine::runtime {

RoomManager::RoomManager() = default;

RoomManager::~RoomManager() = default;

foundation::Result<RoomId> RoomManager::create(time::FixedTimestep timestep, const RoomSetup& setup) {
    if (rooms_.size() >= kMaxRooms) {
        return foundation::Result<RoomId>::fail(foundation::Error{foundation::ErrorCode::Closed, "trop de salles"});
    }
    const RoomId id{nextId_};
    auto room = std::make_unique<Room>(id, timestep);
    if (setup) {
        foundation::Status configured = foundation::okStatus();
        try {
            configured = setup(*room);
        } catch (...) {
            configured = foundation::failStatus(foundation::ErrorCode::InvalidArgument, "configuration de la salle impossible");
        }
        if (!configured.hasValue()) {
            return foundation::Result<RoomId>::fail(configured.error());
        }
    }
    const foundation::Status started = room->start();
    if (!started.hasValue()) {
        return foundation::Result<RoomId>::fail(started.error());
    }
    ++nextId_;
    rooms_.emplace(id, Entry{std::move(room), 0, false});
    return foundation::Result<RoomId>::ok(id);
}

foundation::Status RoomManager::close(RoomId id) {
    const auto found = rooms_.find(id);
    if (found == rooms_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "salle inconnue");
    }
    for (const commands::ClientId client : found->second.room->members()) {
        clientRoom_.erase(client);
    }
    rooms_.erase(found);
    return foundation::okStatus();
}

Room* RoomManager::find(RoomId id) {
    const auto found = rooms_.find(id);
    return found == rooms_.end() ? nullptr : found->second.room.get();
}

const Room* RoomManager::find(RoomId id) const {
    const auto found = rooms_.find(id);
    return found == rooms_.end() ? nullptr : found->second.room.get();
}

foundation::Status RoomManager::join(RoomId id, commands::ClientId client) {
    const auto found = rooms_.find(id);
    if (found == rooms_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "salle inconnue");
    }
    if (found->second.quarantined) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "salle en quarantaine");
    }
    if (clientRoom_.contains(client)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "client deja dans une salle");
    }
    const foundation::Status joined = found->second.room->join(client);
    if (joined.hasValue()) {
        clientRoom_.emplace(client, id);
    }
    return joined;
}

foundation::Status RoomManager::leave(commands::ClientId client) {
    const auto assigned = clientRoom_.find(client);
    if (assigned == clientRoom_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "client dans aucune salle");
    }
    const foundation::Status left = rooms_.at(assigned->second).room->leave(client);
    clientRoom_.erase(assigned);
    return left;
}

std::optional<RoomId> RoomManager::roomOf(commands::ClientId client) const {
    const auto assigned = clientRoom_.find(client);
    if (assigned == clientRoom_.end()) {
        return std::nullopt;
    }
    return assigned->second;
}

RoomsReport RoomManager::tickAll(const time::IClock& clock) {
    RoomsReport report;
    for (auto& [id, entry] : rooms_) {
        if (entry.quarantined) {
            ++report.skipped;
            continue;
        }
        bool failed = false;
        try {
            failed = !entry.room->tick(clock).hasValue();
        } catch (...) {
            failed = true;
        }
        if (!failed) {
            entry.consecutiveFaults = 0;
            ++report.ticked;
            continue;
        }
        ++report.faulted;
        if (++entry.consecutiveFaults >= kMaxConsecutiveRoomFaults) {
            entry.quarantined = true;
        }
    }
    return report;
}

std::size_t RoomManager::roomCount() const noexcept {
    return rooms_.size();
}

std::size_t RoomManager::quarantinedCount() const noexcept {
    std::size_t count = 0;
    for (const auto& [id, entry] : rooms_) {
        if (entry.quarantined) {
            ++count;
        }
    }
    return count;
}

bool RoomManager::quarantined(RoomId id) const {
    const auto found = rooms_.find(id);
    return found != rooms_.end() && found->second.quarantined;
}

std::vector<RoomId> RoomManager::ids() const {
    std::vector<RoomId> result;
    result.reserve(rooms_.size());
    for (const auto& [id, entry] : rooms_) {
        result.push_back(id);
    }
    return result;
}

}
