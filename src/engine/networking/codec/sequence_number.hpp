#pragma once

#include <cstdint>

namespace engine::networking {

[[nodiscard]] std::uint16_t sequenceDistance(std::uint16_t newer, std::uint16_t older) noexcept;
[[nodiscard]] bool isSequenceMoreRecent(std::uint16_t candidate, std::uint16_t reference) noexcept;

}
