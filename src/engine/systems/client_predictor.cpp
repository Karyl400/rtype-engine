#include "engine/systems/client_predictor.hpp"

#include <cmath>

namespace engine::systems {

ClientPredictor::ClientPredictor() = default;

ClientPredictor::ClientPredictor(std::size_t maxPendingInputs)
    : maxPendingInputs_(maxPendingInputs) {}

void ClientPredictor::recordInput(std::uint32_t sequence, foundation::Vec2 velocity, std::chrono::nanoseconds dt) {
    if (pendingInputs_.size() >= maxPendingInputs_) {
        pendingInputs_.pop_front();
    }
    pendingInputs_.push_back(PendingInput{sequence, velocity, dt});
}

void ClientPredictor::predict(foundation::Vec2& inOutPosition, foundation::Vec2 velocity, std::chrono::nanoseconds dt) {
    const float dtSec = static_cast<float>(std::chrono::duration<double>(dt).count());
    inOutPosition.x += velocity.x * dtSec;
    inOutPosition.y += velocity.y * dtSec;
}

bool ClientPredictor::reconcile(foundation::Vec2& inOutPosition, std::uint32_t acknowledgedSequence, foundation::Vec2 serverPosition, float errorThreshold) {
    while (!pendingInputs_.empty() && pendingInputs_.front().sequence <= acknowledgedSequence) {
        pendingInputs_.pop_front();
    }

    foundation::Vec2 replayedPosition = serverPosition;
    for (const auto& input : pendingInputs_) {
        const float dtSec = static_cast<float>(std::chrono::duration<double>(input.dt).count());
        replayedPosition.x += input.velocity.x * dtSec;
        replayedPosition.y += input.velocity.y * dtSec;
    }

    const float dx = inOutPosition.x - replayedPosition.x;
    const float dy = inOutPosition.y - replayedPosition.y;
    const float errorDist = std::sqrt(dx * dx + dy * dy);

    if (errorDist > errorThreshold) {
        errorOffset_.x += inOutPosition.x - replayedPosition.x;
        errorOffset_.y += inOutPosition.y - replayedPosition.y;
        inOutPosition = replayedPosition;
        return true;
    }

    return false;
}

void ClientPredictor::updateVisualReconciliation(std::chrono::nanoseconds dt, float decayRate) {
    const float dtSec = static_cast<float>(std::chrono::duration<double>(dt).count());
    const float decay = std::exp(-decayRate * dtSec);
    errorOffset_.x *= decay;
    errorOffset_.y *= decay;

    if ((errorOffset_.x * errorOffset_.x + errorOffset_.y * errorOffset_.y) < 0.0001F) {
        errorOffset_ = foundation::Vec2{0.0F, 0.0F};
    }
}

foundation::Vec2 ClientPredictor::visualPosition(foundation::Vec2 simulationPosition) const {
    return foundation::Vec2{simulationPosition.x + errorOffset_.x, simulationPosition.y + errorOffset_.y};
}

std::size_t ClientPredictor::pendingInputCount() const {
    return pendingInputs_.size();
}

foundation::Vec2 ClientPredictor::currentErrorOffset() const {
    return errorOffset_;
}

bool ClientPredictor::hasActiveError() const {
    return errorOffset_.x != 0.0F || errorOffset_.y != 0.0F;
}

void ClientPredictor::clear() {
    pendingInputs_.clear();
    errorOffset_ = foundation::Vec2{0.0F, 0.0F};
}

}
