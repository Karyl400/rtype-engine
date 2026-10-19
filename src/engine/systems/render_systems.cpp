#include "engine/systems/render_systems.hpp"

#include <algorithm>
#include <cmath>
#include <span>
#include <tuple>
#include <utility>

#include "engine/systems/components.hpp"

namespace engine::systems {

namespace {

bool finite(foundation::Vec2 v) {
    return std::isfinite(v.x) && std::isfinite(v.y);
}

}

foundation::Status validate(const rendering::RenderCommand& command) {
    if (command.texture.value == 0) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "texture absente");
    }
    const auto& transform = command.transform;
    if (!finite(transform.position) || !finite(transform.scale) || !std::isfinite(transform.rotationRadians)) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "transformation non finie");
    }
    if (transform.scale.x == 0.0F || transform.scale.y == 0.0F) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "echelle nulle");
    }
    const auto& source = command.source;
    const bool finiteSource =
        std::isfinite(source.x) && std::isfinite(source.y) && std::isfinite(source.width) && std::isfinite(source.height);
    if (!finiteSource || source.width <= 0.0F || source.height <= 0.0F) {
        return foundation::failStatus(foundation::ErrorCode::InvalidArgument, "rectangle source invalide");
    }
    return foundation::okStatus();
}

std::vector<rendering::RenderCommand> collectRenderCommands(ecs::World& world) {
    struct Item {
        rendering::Layer layer;
        ecs::Entity entity;
        rendering::RenderCommand command;
    };
    std::vector<Item> items;
    world.each<Position, Sprite>([&items](ecs::Entity entity, Position& position, Sprite& sprite) {
        rendering::RenderCommand command;
        command.texture = sprite.texture;
        command.transform.position = position.value;
        command.tint = sprite.tint;
        command.source = sprite.source;
        command.layer = sprite.layer;
        items.push_back(Item{sprite.layer, entity, command});
    });
    std::sort(items.begin(), items.end(),
              [](const Item& a, const Item& b) { return std::tie(a.layer, a.entity) < std::tie(b.layer, b.entity); });
    std::vector<rendering::RenderCommand> commands;
    commands.reserve(items.size());
    for (Item& item : items) {
        commands.push_back(item.command);
    }
    return commands;
}

RenderReport renderFrame(ecs::World& world, rendering::IRenderer& renderer) {
    std::vector<rendering::RenderCommand> commands = collectRenderCommands(world);
    RenderReport report;
    std::erase_if(commands, [&report](const rendering::RenderCommand& command) {
        if (validate(command).hasValue()) {
            return false;
        }
        ++report.rejected;
        return true;
    });
    report.submitted = commands.size();
    renderer.beginFrame();
    renderer.submit(std::span<const rendering::RenderCommand>(commands));
    renderer.endFrame();
    return report;
}

}
