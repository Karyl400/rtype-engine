#pragma once

namespace engine::foundation {

struct Vec2 {
    float x = 0.0F;
    float y = 0.0F;

    friend bool operator==(const Vec2&, const Vec2&) = default;
};

[[nodiscard]] Vec2 operator+(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator-(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 operator*(Vec2 v, float s);
[[nodiscard]] Vec2 operator*(float s, Vec2 v);

[[nodiscard]] float dot(Vec2 a, Vec2 b);
[[nodiscard]] float lengthSquared(Vec2 v);
[[nodiscard]] float length(Vec2 v);
[[nodiscard]] float distance(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 normalized(Vec2 v);

}
