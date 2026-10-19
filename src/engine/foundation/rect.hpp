#pragma once

namespace engine::foundation {

struct Rect {
    float x = 0.0F;
    float y = 0.0F;
    float width = 0.0F;
    float height = 0.0F;

    friend bool operator==(const Rect&, const Rect&) = default;
};

}
