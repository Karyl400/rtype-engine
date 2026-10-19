#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>

#include "engine/foundation/result.hpp"

namespace engine::runtime {

enum class ServerMode { Simulated, Realtime, Threaded, Serve };

inline constexpr std::uint16_t kDefaultServerPort = 4242;
inline constexpr std::uint32_t kDefaultRoomCount = 2;

struct ServerOptions {
    ServerMode mode = ServerMode::Simulated;
    std::uint64_t ticks = 0;
    std::uint16_t port = kDefaultServerPort;
    std::string bind = "0.0.0.0";
    std::uint32_t rooms = kDefaultRoomCount;
    std::string content = "content";
    std::uint64_t seed = 2026;
    std::uint32_t lobbySeconds = 10;
};

[[nodiscard]] foundation::Result<ServerOptions> parseServerOptions(std::span<const std::string_view> arguments);

}
