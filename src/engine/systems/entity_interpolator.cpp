#include "engine/systems/entity_interpolator.hpp"

#include <algorithm>
#include <cmath>

#include "engine/systems/components.hpp"

namespace engine::systems {

namespace {

constexpr std::size_t kMaxSnapshots = 64;
constexpr float kJitterFilterAlpha = 0.1F;

const EntityState* findEntity(const WorldSnapshot& snapshot, ecs::Entity entity) {
    for (const auto& state : snapshot.entities) {
        if (state.entity == entity) {
            return &state;
        }
    }
    return nullptr;
}

}

EntityInterpolator::EntityInterpolator() = default;

EntityInterpolator::EntityInterpolator(std::chrono::microseconds interpolationDelay)
    : interpolationDelay_(interpolationDelay) {}

void EntityInterpolator::setInterpolationDelay(std::chrono::microseconds delay) {
    interpolationDelay_ = delay;
}

std::chrono::microseconds EntityInterpolator::interpolationDelay() const {
    return interpolationDelay_;
}

void EntityInterpolator::pushSnapshot(WorldSnapshot snapshot) {
    if (lastArrivalTime_.count() > 0) {
        const auto deltaMicro = snapshot.timestamp - lastArrivalTime_;
        const float deltaMs = std::chrono::duration<float, std::milli>(deltaMicro).count();
        constexpr float kNominalIntervalMs = 16.666F;
        const float instantJitter = std::abs(deltaMs - kNominalIntervalMs);
        estimatedJitterMs_ = ((1.0F - kJitterFilterAlpha) * estimatedJitterMs_) + (kJitterFilterAlpha * instantJitter);
    }
    lastArrivalTime_ = snapshot.timestamp;

    if (snapshots_.empty() || snapshot.timestamp >= snapshots_.back().timestamp) {
        snapshots_.push_back(std::move(snapshot));
    } else {
        auto it = std::lower_bound(snapshots_.begin(), snapshots_.end(), snapshot.timestamp,
            [](const WorldSnapshot& s, std::chrono::microseconds t) {
                return s.timestamp < t;
            });
        snapshots_.insert(it, std::move(snapshot));
    }

    while (snapshots_.size() > kMaxSnapshots) {
        snapshots_.pop_front();
    }
}

void EntityInterpolator::clear() {
    snapshots_.clear();
    lastArrivalTime_ = std::chrono::microseconds{0};
    estimatedJitterMs_ = 0.0F;
}

std::size_t EntityInterpolator::snapshotCount() const {
    return snapshots_.size();
}

std::optional<foundation::Vec2> EntityInterpolator::samplePosition(
    ecs::Entity entity, std::chrono::microseconds renderTime) const {
    if (snapshots_.empty()) {
        return std::nullopt;
    }

    if (snapshots_.size() == 1 || renderTime <= snapshots_.front().timestamp) {
        const auto* state = findEntity(snapshots_.front(), entity);
        if (state != nullptr) {
            return state->position;
        }
        return std::nullopt;
    }

    if (renderTime >= snapshots_.back().timestamp) {
        const auto* state = findEntity(snapshots_.back(), entity);
        if (state != nullptr) {
            const auto dtMicro = renderTime - snapshots_.back().timestamp;
            const float dtSec = std::chrono::duration<float>(dtMicro).count();
            return state->position + (state->velocity * dtSec);
        }
        return std::nullopt;
    }

    for (std::size_t i = 0; i + 1 < snapshots_.size(); ++i) {
        const auto& s0 = snapshots_[i];
        const auto& s1 = snapshots_[i + 1];

        if (s0.timestamp <= renderTime && renderTime <= s1.timestamp) {
            const auto* state0 = findEntity(s0, entity);
            const auto* state1 = findEntity(s1, entity);

            if (state0 != nullptr && state1 != nullptr) {
                const auto rangeMicro = (s1.timestamp - s0.timestamp).count();
                if (rangeMicro <= 0) {
                    return state1->position;
                }
                const auto progressMicro = (renderTime - s0.timestamp).count();
                const float factor = static_cast<float>(progressMicro) / static_cast<float>(rangeMicro);
                const float clamped = std::clamp(factor, 0.0F, 1.0F);
                return state0->position + ((state1->position - state0->position) * clamped);
            }
            if (state1 != nullptr) {
                return state1->position;
            }
            if (state0 != nullptr) {
                return state0->position;
            }
            return std::nullopt;
        }
    }

    const auto* fallback = findEntity(snapshots_.back(), entity);
    if (fallback != nullptr) {
        return fallback->position;
    }
    return std::nullopt;
}

void EntityInterpolator::update(ecs::World& world, std::chrono::microseconds renderTime) {
    const auto effectiveTime = (renderTime >= interpolationDelay_)
        ? (renderTime - interpolationDelay_)
        : std::chrono::microseconds{0};

    world.each<Position>([this, effectiveTime](ecs::Entity entity, Position& pos) {
        const auto sampled = samplePosition(entity, effectiveTime);
        if (sampled.has_value()) {
            pos.value = sampled.value();
        }
    });
}

float EntityInterpolator::estimatedJitterMs() const {
    return estimatedJitterMs_;
}

}
