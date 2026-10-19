#pragma once

#include "engine/rendering/api/iwindow.hpp"

namespace engine::rendering {

class NullWindow final : public IWindow {
public:
    [[nodiscard]] bool isOpen() const override;
    void pollEvents(EventQueue& queue) override;
    void present() override;

    void requestClose();

private:
    bool open_ = true;
    bool closeRequested_ = false;
};

}
