#include "engine/commands/command_processor.hpp"

#include <utility>

namespace engine::commands {

CommandProcessor::CommandProcessor() = default;

CommandProcessor::~CommandProcessor() = default;

foundation::Status CommandProcessor::addHandler(std::type_index type, Handler handler) {
    if (!handler) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "gestionnaire vide");
    }
    if (!handlers_.emplace(type, std::move(handler)).second) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "commande deja enregistree");
    }
    return foundation::okStatus();
}

foundation::Status CommandProcessor::enqueue(ClientId client, std::type_index type, std::any payload) {
    if (handlers_.find(type) == handlers_.end()) {
        return foundation::failStatus(foundation::ErrorCode::NotFound, "commande inconnue");
    }
    if (queue_.size() >= kMaxPendingTotal) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "file de commandes pleine");
    }
    std::size_t& count = perClient_[client];
    if (count >= kMaxPendingPerClient) {
        return foundation::failStatus(foundation::ErrorCode::Closed, "trop de commandes en attente pour ce client");
    }
    ++count;
    queue_.push_back(Entry{client, type, std::move(payload)});
    return foundation::okStatus();
}

void CommandProcessor::run(Entry& entry, ecs::World& world, ProcessReport& report) {
    try {
        const foundation::Status verdict = handlers_.at(entry.type)(world, entry.client, entry.payload);
        if (verdict.hasValue()) {
            ++report.executed;
        } else {
            ++report.rejected;
        }
    } catch (...) {
        ++report.faulted;
    }
}

ProcessReport CommandProcessor::process(ecs::World& world) {
    std::vector<Entry> batch;
    batch.swap(queue_);
    perClient_.clear();
    ProcessReport report;
    for (Entry& entry : batch) {
        run(entry, world, report);
    }
    return report;
}

std::size_t CommandProcessor::dropClient(ClientId client) {
    const std::size_t before = queue_.size();
    std::erase_if(queue_, [client](const Entry& entry) { return entry.client == client; });
    perClient_.erase(client);
    return before - queue_.size();
}

std::size_t CommandProcessor::pending() const noexcept {
    return queue_.size();
}

std::size_t CommandProcessor::pending(ClientId client) const {
    const auto found = perClient_.find(client);
    return found == perClient_.end() ? 0 : found->second;
}

}
