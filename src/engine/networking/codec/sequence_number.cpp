#include "engine/networking/codec/sequence_number.hpp"

namespace engine::networking {

namespace {

constexpr std::uint16_t kSequenceHalfRange = 0x8000;

}

std::uint16_t sequenceDistance(std::uint16_t newer, std::uint16_t older) noexcept {
    return static_cast<std::uint16_t>(newer - older);
}

bool isSequenceMoreRecent(std::uint16_t candidate, std::uint16_t reference) noexcept {
    if (candidate == reference) {
        return false;
    }
    return sequenceDistance(candidate, reference) < kSequenceHalfRange;
}

}
