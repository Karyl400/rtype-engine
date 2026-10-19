#include "engine/rendering/api/accessibility.hpp"

#include <algorithm>
#include <cmath>

namespace engine::rendering {

namespace {

std::uint8_t clampColor(float val) {
    const float clamped = std::clamp(val, 0.0F, 255.0F);
    return static_cast<std::uint8_t>(std::round(clamped));
}

}

AccessibilityManager::AccessibilityManager() = default;

AccessibilityManager::AccessibilityManager(const AccessibilityConfig& config)
    : config_(config) {
    config_.uiScale = std::clamp(config_.uiScale, 0.5F, 4.0F);
}

void AccessibilityManager::setFilter(ColorblindFilter filter) {
    config_.colorFilter = filter;
}

ColorblindFilter AccessibilityManager::filter() const {
    return config_.colorFilter;
}

void AccessibilityManager::setReducedMotion(bool enabled) {
    config_.reducedMotion = enabled;
}

bool AccessibilityManager::reducedMotion() const {
    return config_.reducedMotion;
}

void AccessibilityManager::setUiScale(float scale) {
    config_.uiScale = std::clamp(scale, 0.5F, 4.0F);
}

float AccessibilityManager::uiScale() const {
    return config_.uiScale;
}

foundation::Color AccessibilityManager::applyFilter(foundation::Color color) const {
    const float r = static_cast<float>(color.r);
    const float g = static_cast<float>(color.g);
    const float b = static_cast<float>(color.b);

    switch (config_.colorFilter) {
        case ColorblindFilter::None:
            return color;

        case ColorblindFilter::Protanopia: {
            const float nr = 0.56667F * r + 0.43333F * g;
            const float ng = 0.55833F * r + 0.44167F * g;
            const float nb = 0.24167F * g + 0.75833F * b;
            return foundation::Color{clampColor(nr), clampColor(ng), clampColor(nb), color.a};
        }

        case ColorblindFilter::Deuteranopia: {
            const float nr = 0.625F * r + 0.375F * g;
            const float ng = 0.700F * r + 0.300F * g;
            const float nb = 0.300F * g + 0.700F * b;
            return foundation::Color{clampColor(nr), clampColor(ng), clampColor(nb), color.a};
        }

        case ColorblindFilter::Tritanopia: {
            const float nr = 0.950F * r + 0.050F * g;
            const float ng = 0.43333F * g + 0.56667F * b;
            const float nb = 0.475F * g + 0.525F * b;
            return foundation::Color{clampColor(nr), clampColor(ng), clampColor(nb), color.a};
        }

        case ColorblindFilter::HighContrast: {
            const float lum = 0.299F * r + 0.587F * g + 0.114F * b;
            if (lum >= 128.0F) {
                return foundation::Color{255, 255, 100, color.a};
            }
            return foundation::Color{15, 15, 25, color.a};
        }
    }

    return color;
}

foundation::Rect AccessibilityManager::scaleRect(foundation::Rect base) const {
    return foundation::Rect{
        base.x * config_.uiScale,
        base.y * config_.uiScale,
        base.width * config_.uiScale,
        base.height * config_.uiScale
    };
}

foundation::Vec2 AccessibilityManager::scaleVec(foundation::Vec2 base) const {
    return foundation::Vec2{
        base.x * config_.uiScale,
        base.y * config_.uiScale
    };
}

void AccessibilityManager::setConfig(const AccessibilityConfig& config) {
    config_ = config;
    config_.uiScale = std::clamp(config_.uiScale, 0.5F, 4.0F);
}

const AccessibilityConfig& AccessibilityManager::config() const {
    return config_;
}

}
