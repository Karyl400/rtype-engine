#include "engine/runtime/server_options.hpp"

#include <charconv>
#include <limits>
#include <system_error>

#include "engine/runtime/room_manager.hpp"

namespace engine::runtime {

namespace {

foundation::Error invalid(const char* message) {
    return foundation::Error{foundation::ErrorCode::InvalidArgument, message};
}

template <typename Integer>
bool parseNumber(std::string_view text, Integer& value) {
    Integer parsed{};
    const auto result = std::from_chars(text.data(), text.data() + text.size(), parsed);
    if (result.ec != std::errc() || result.ptr != text.data() + text.size()) {
        return false;
    }
    value = parsed;
    return true;
}

}

foundation::Result<ServerOptions> parseServerOptions(std::span<const std::string_view> arguments) {
    using Failure = foundation::Result<ServerOptions>;
    ServerOptions options;
    if (arguments.empty()) {
        return Failure::ok(options);
    }
    const std::string_view first = arguments.front();
    if (first == "--realtime" || first == "--threaded") {
        if (arguments.size() != 2 || !parseNumber(arguments[1], options.ticks) || options.ticks == 0) {
            return Failure::fail(invalid("nombre de ticks invalide"));
        }
        options.mode = first == "--threaded" ? ServerMode::Threaded : ServerMode::Realtime;
        return Failure::ok(options);
    }
    if (first != "--serve") {
        return Failure::fail(invalid("option inconnue"));
    }
    options.mode = ServerMode::Serve;
    for (std::size_t index = 1; index < arguments.size(); index += 2) {
        if (index + 1 >= arguments.size()) {
            return Failure::fail(invalid("valeur manquante"));
        }
        const std::string_view name = arguments[index];
        const std::string_view value = arguments[index + 1];
        if (name == "--port") {
            std::uint32_t port = 0;
            if (!parseNumber(value, port) || port == 0 || port > std::numeric_limits<std::uint16_t>::max()) {
                return Failure::fail(invalid("port invalide"));
            }
            options.port = static_cast<std::uint16_t>(port);
        } else if (name == "--bind") {
            if (value.empty()) {
                return Failure::fail(invalid("adresse vide"));
            }
            options.bind = std::string(value);
        } else if (name == "--rooms") {
            if (!parseNumber(value, options.rooms) || options.rooms == 0 || options.rooms > kMaxRooms) {
                return Failure::fail(invalid("nombre de salles invalide"));
            }
        } else if (name == "--content") {
            if (value.empty()) {
                return Failure::fail(invalid("dossier de contenu vide"));
            }
            options.content = std::string(value);
        } else if (name == "--lobby") {
            if (!parseNumber(value, options.lobbySeconds) || options.lobbySeconds > 3600) {
                return Failure::fail(invalid("delai de salle d'attente invalide"));
            }
        } else if (name == "--seed") {
            if (!parseNumber(value, options.seed)) {
                return Failure::fail(invalid("graine invalide"));
            }
        } else if (name == "--ticks") {
            if (!parseNumber(value, options.ticks)) {
                return Failure::fail(invalid("nombre de ticks invalide"));
            }
        } else {
            return Failure::fail(invalid("option inconnue"));
        }
    }
    return Failure::ok(options);
}

}
