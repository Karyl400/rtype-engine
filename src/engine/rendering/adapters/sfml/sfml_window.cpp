#include "engine/rendering/adapters/sfml/sfml_window.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace engine::rendering {

struct SfmlWindow::Impl {
    sf::RenderWindow window;
    std::uint32_t width{0};
    std::uint32_t height{0};

    Impl(std::uint32_t w, std::uint32_t h, std::string_view title)
        : window(sf::VideoMode({w, h}), sf::String::fromUtf8(title.begin(), title.end())),
          width(w),
          height(h) {
        window.setVerticalSyncEnabled(true);
    }
};

SfmlWindow::SfmlWindow(std::uint32_t width, std::uint32_t height, std::string_view title)
    : impl_(std::make_unique<Impl>(width, height, title)) {}

SfmlWindow::~SfmlWindow() = default;

bool SfmlWindow::isOpen() const {
    return impl_->window.isOpen();
}

void SfmlWindow::pollEvents(EventQueue& queue) {
    while (const std::optional event = impl_->window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            queue.push_back(WindowEvent{WindowEvent::Type::Closed, impl_->width, impl_->height});
            impl_->window.close();
            continue;
        }
        if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            impl_->width = resized->size.x;
            impl_->height = resized->size.y;
            queue.push_back(WindowEvent{WindowEvent::Type::Resized, impl_->width, impl_->height});
            continue;
        }
        if (event->is<sf::Event::FocusLost>()) {
            queue.push_back(WindowEvent{WindowEvent::Type::FocusLost, impl_->width, impl_->height});
            continue;
        }
        if (event->is<sf::Event::FocusGained>()) {
            queue.push_back(WindowEvent{WindowEvent::Type::FocusGained, impl_->width, impl_->height});
            continue;
        }
    }
}

void SfmlWindow::present() {
    impl_->window.display();
}

void SfmlWindow::close() {
    impl_->window.close();
}

void* SfmlWindow::nativeHandle() const {
    return &impl_->window;
}

std::uint32_t SfmlWindow::width() const {
    return impl_->width;
}

std::uint32_t SfmlWindow::height() const {
    return impl_->height;
}

}
