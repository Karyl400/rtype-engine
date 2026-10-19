#pragma once

#include <cstdint>

#include "engine/foundation/color.hpp"
#include "engine/foundation/rect.hpp"
#include "engine/foundation/vec2.hpp"

namespace engine::rendering {

enum class ColorblindFilter : std::uint8_t {
    None = 0,
    Protanopia,
    Deuteranopia,
    Tritanopia,
    HighContrast
};

struct AccessibilityConfig {
    ColorblindFilter colorFilter = ColorblindFilter::None;
    bool reducedMotion = false;
    float uiScale = 1.0F;
};

class AccessibilityManager {
public:
    AccessibilityManager();
    explicit AccessibilityManager(const AccessibilityConfig& config);

    void setFilter(ColorblindFilter filter);
    [[nodiscard]] ColorblindFilter filter() const;

    void setReducedMotion(bool enabled);
    [[nodiscard]] bool reducedMotion() const;

    void setUiScale(float scale);
    [[nodiscard]] float uiScale() const;

    [[nodiscard]] foundation::Color applyFilter(foundation::Color color) const;
    [[nodiscard]] foundation::Rect scaleRect(foundation::Rect base) const;
    [[nodiscard]] foundation::Vec2 scaleVec(foundation::Vec2 base) const;

    void setConfig(const AccessibilityConfig& config);
    [[nodiscard]] const AccessibilityConfig& config() const;

private:
    AccessibilityConfig config_{};
};

}
