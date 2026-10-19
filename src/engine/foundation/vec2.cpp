#include "engine/foundation/vec2.hpp"

#include <cmath>

namespace engine::foundation {

Vec2 operator+(Vec2 a, Vec2 b) {
    return {a.x + b.x, a.y + b.y};
}

Vec2 operator-(Vec2 a, Vec2 b) {
    return {a.x - b.x, a.y - b.y};
}

Vec2 operator*(Vec2 v, float s) {
    return {v.x * s, v.y * s};
}

Vec2 operator*(float s, Vec2 v) {
    return v * s;
}

float dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

float lengthSquared(Vec2 v) {
    return dot(v, v);
}

float length(Vec2 v) {
    return std::hypot(v.x, v.y);
}

float distance(Vec2 a, Vec2 b) {
    return length(b - a);
}

Vec2 normalized(Vec2 v) {
    const float len = length(v);
    if (!(len > 0.0F) || !std::isfinite(len)) {
        return {};
    }
    return {v.x / len, v.y / len};
}

}
