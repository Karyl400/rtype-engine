#pragma once

#include <memory>

#include "engine/input/api/iinput.hpp"

namespace engine::input {

enum class KeyCode {
    Unknown,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, Space, Enter, Backspace, Tab,
    Up, Down, Left, Right
};

class SfmlInput final : public IInput {
public:
    SfmlInput();
    ~SfmlInput() override;

    void update() override;
    [[nodiscard]] bool isDown(InputAction action) const override;
    [[nodiscard]] bool wasPressed(InputAction action) const override;

    void bindAction(InputAction action, KeyCode key);
    void unbindAction(InputAction action);
    void resetDefaultBindings();

    void simulateKeyPress(KeyCode key, bool pressed);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
