#include "engine/rendering/adapters/null/null_window.hpp"

namespace engine::rendering {

bool NullWindow::isOpen() const {
    return open_;
}

void NullWindow::pollEvents(EventQueue& queue) {
    if (closeRequested_ && open_) {
        queue.push_back(WindowEvent{WindowEvent::Type::Closed, 0, 0});
        open_ = false;
    }
}

void NullWindow::present() {}

void NullWindow::requestClose() {
    closeRequested_ = true;
}

}
