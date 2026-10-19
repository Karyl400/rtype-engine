#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>

#include "engine/foundation/vec2.hpp"

namespace engine::systems {

struct PendingInput {
    std::uint32_t sequence = 0;
    foundation::Vec2 velocity{};
    std::chrono::nanoseconds dt{0};
};

class ClientPredictor {
public:
    ClientPredictor();
    explicit ClientPredictor(std::size_t maxPendingInputs);

    void recordInput(std::uint32_t sequence, foundation::Vec2 velocity, std::chrono::nanoseconds dt);
    void predict(foundation::Vec2& inOutPosition, foundation::Vec2 velocity, std::chrono::nanoseconds dt);
    bool reconcile(foundation::Vec2& inOutPosition, std::uint32_t acknowledgedSequence, foundation::Vec2 serverPosition, float errorThreshold = 0.5F);

    void updateVisualReconciliation(std::chrono::nanoseconds dt, float decayRate = 15.0F);
    [[nodiscard]] foundation::Vec2 visualPosition(foundation::Vec2 simulationPosition) const;

    [[nodiscard]] std::size_t pendingInputCount() const;
    [[nodiscard]] foundation::Vec2 currentErrorOffset() const;
    [[nodiscard]] bool hasActiveError() const;
    void clear();

private:
    std::size_t maxPendingInputs_ = 128;
    std::deque<PendingInput> pendingInputs_;
    foundation::Vec2 errorOffset_{};
};

}
