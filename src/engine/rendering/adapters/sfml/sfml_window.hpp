#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "engine/foundation/noncopyable.hpp"
#include "engine/rendering/api/iwindow.hpp"

namespace engine::rendering {

class SfmlWindow final : public IWindow {
public:
    SfmlWindow(std::uint32_t width, std::uint32_t height, std::string_view title);
    ~SfmlWindow() override;

    [[nodiscard]] bool isOpen() const override;
    void pollEvents(EventQueue& queue) override;
    void present() override;

    void close();
    [[nodiscard]] void* nativeHandle() const;
    [[nodiscard]] std::uint32_t width() const;
    [[nodiscard]] std::uint32_t height() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
