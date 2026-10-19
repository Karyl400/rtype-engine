#include "engine/input/adapters/sfml/sfml_input.hpp"

#include <unordered_map>
#include <unordered_set>

#include <SFML/Window/Keyboard.hpp>

namespace engine::input {

namespace {

sf::Keyboard::Key toSfmlKey(KeyCode code) {
    switch (code) {
        case KeyCode::A: return sf::Keyboard::Key::A;
        case KeyCode::B: return sf::Keyboard::Key::B;
        case KeyCode::C: return sf::Keyboard::Key::C;
        case KeyCode::D: return sf::Keyboard::Key::D;
        case KeyCode::E: return sf::Keyboard::Key::E;
        case KeyCode::F: return sf::Keyboard::Key::F;
        case KeyCode::G: return sf::Keyboard::Key::G;
        case KeyCode::H: return sf::Keyboard::Key::H;
        case KeyCode::I: return sf::Keyboard::Key::I;
        case KeyCode::J: return sf::Keyboard::Key::J;
        case KeyCode::K: return sf::Keyboard::Key::K;
        case KeyCode::L: return sf::Keyboard::Key::L;
        case KeyCode::M: return sf::Keyboard::Key::M;
        case KeyCode::N: return sf::Keyboard::Key::N;
        case KeyCode::O: return sf::Keyboard::Key::O;
        case KeyCode::P: return sf::Keyboard::Key::P;
        case KeyCode::Q: return sf::Keyboard::Key::Q;
        case KeyCode::R: return sf::Keyboard::Key::R;
        case KeyCode::S: return sf::Keyboard::Key::S;
        case KeyCode::T: return sf::Keyboard::Key::T;
        case KeyCode::U: return sf::Keyboard::Key::U;
        case KeyCode::V: return sf::Keyboard::Key::V;
        case KeyCode::W: return sf::Keyboard::Key::W;
        case KeyCode::X: return sf::Keyboard::Key::X;
        case KeyCode::Y: return sf::Keyboard::Key::Y;
        case KeyCode::Z: return sf::Keyboard::Key::Z;
        case KeyCode::Num0: return sf::Keyboard::Key::Num0;
        case KeyCode::Num1: return sf::Keyboard::Key::Num1;
        case KeyCode::Num2: return sf::Keyboard::Key::Num2;
        case KeyCode::Num3: return sf::Keyboard::Key::Num3;
        case KeyCode::Num4: return sf::Keyboard::Key::Num4;
        case KeyCode::Num5: return sf::Keyboard::Key::Num5;
        case KeyCode::Num6: return sf::Keyboard::Key::Num6;
        case KeyCode::Num7: return sf::Keyboard::Key::Num7;
        case KeyCode::Num8: return sf::Keyboard::Key::Num8;
        case KeyCode::Num9: return sf::Keyboard::Key::Num9;
        case KeyCode::Escape: return sf::Keyboard::Key::Escape;
        case KeyCode::Space: return sf::Keyboard::Key::Space;
        case KeyCode::Enter: return sf::Keyboard::Key::Enter;
        case KeyCode::Backspace: return sf::Keyboard::Key::Backspace;
        case KeyCode::Tab: return sf::Keyboard::Key::Tab;
        case KeyCode::Up: return sf::Keyboard::Key::Up;
        case KeyCode::Down: return sf::Keyboard::Key::Down;
        case KeyCode::Left: return sf::Keyboard::Key::Left;
        case KeyCode::Right: return sf::Keyboard::Key::Right;
        default: return sf::Keyboard::Key::Unknown;
    }
}

}

struct SfmlInput::Impl {
    std::unordered_map<InputAction, std::vector<KeyCode>> bindings{};
    std::unordered_set<KeyCode> simulatedPressed{};
    std::unordered_map<InputAction, bool> currentState{};
    std::unordered_map<InputAction, bool> previousState{};

    Impl() {
        setDefaultBindings();
    }

    void setDefaultBindings() {
        bindings.clear();
        bindings[InputAction{1}] = {KeyCode::Up, KeyCode::Z};
        bindings[InputAction{2}] = {KeyCode::Down, KeyCode::S};
        bindings[InputAction{3}] = {KeyCode::Left, KeyCode::Q};
        bindings[InputAction{4}] = {KeyCode::Right, KeyCode::D};
        bindings[InputAction{5}] = {KeyCode::Space};
        bindings[InputAction{6}] = {KeyCode::F};
    }

    bool isKeyPressed(KeyCode key) const {
        if (simulatedPressed.contains(key)) {
            return true;
        }
        const sf::Keyboard::Key sfKey = toSfmlKey(key);
        if (sfKey == sf::Keyboard::Key::Unknown) {
            return false;
        }
        return sf::Keyboard::isKeyPressed(sfKey);
    }
};

SfmlInput::SfmlInput() : impl_(std::make_unique<Impl>()) {}

SfmlInput::~SfmlInput() = default;

void SfmlInput::update() {
    impl_->previousState = impl_->currentState;
    impl_->currentState.clear();
    for (const auto& [action, keys] : impl_->bindings) {
        bool active = false;
        for (const auto key : keys) {
            if (impl_->isKeyPressed(key)) {
                active = true;
                break;
            }
        }
        impl_->currentState[action] = active;
    }
}

bool SfmlInput::isDown(InputAction action) const {
    const auto it = impl_->currentState.find(action);
    if (it == impl_->currentState.end()) {
        return false;
    }
    return it->second;
}

bool SfmlInput::wasPressed(InputAction action) const {
    const auto itCurrent = impl_->currentState.find(action);
    if (itCurrent == impl_->currentState.end() || !itCurrent->second) {
        return false;
    }
    const auto itPrev = impl_->previousState.find(action);
    const bool wasDown = (itPrev != impl_->previousState.end()) && itPrev->second;
    return !wasDown;
}

void SfmlInput::bindAction(InputAction action, KeyCode key) {
    impl_->bindings[action].push_back(key);
}

void SfmlInput::unbindAction(InputAction action) {
    impl_->bindings.erase(action);
    impl_->currentState.erase(action);
}

void SfmlInput::resetDefaultBindings() {
    impl_->setDefaultBindings();
}

void SfmlInput::simulateKeyPress(KeyCode key, bool pressed) {
    if (pressed) {
        impl_->simulatedPressed.insert(key);
        return;
    }
    impl_->simulatedPressed.erase(key);
}

}
