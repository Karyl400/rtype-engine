#pragma once

#include <type_traits>
#include <utility>

namespace engine::commands {

template <typename Command, typename Validator, typename Executor>
foundation::Status CommandProcessor::registerCommand(Validator validator, Executor executor) {
    static_assert(std::is_copy_constructible_v<Command>, "une commande doit etre copiable");
    return addHandler(std::type_index(typeid(Command)),
                      [validator = std::move(validator), executor = std::move(executor)](ecs::World& world, ClientId client,
                                                                                         const std::any& payload) -> foundation::Status {
                          const Command& command = std::any_cast<const Command&>(payload);
                          const ecs::World& view = world;
                          foundation::Status verdict = validator(view, client, command);
                          if (!verdict.hasValue()) {
                              return verdict;
                          }
                          executor(world, client, command);
                          return foundation::okStatus();
                      });
}

template <typename Command>
foundation::Status CommandProcessor::submit(ClientId client, Command command) {
    static_assert(std::is_copy_constructible_v<Command>, "une commande doit etre copiable");
    return enqueue(client, std::type_index(typeid(Command)), std::any(std::move(command)));
}

}
