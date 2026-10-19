#pragma once

#include <cstdint>

namespace engine::foundation {

struct Color {
    std::uint8_t r = 255;
    std::uint8_t g = 255;
    std::uint8_t b = 255;
    std::uint8_t a = 255;

    friend bool operator==(const Color&, const Color&) = default;
};

}
