#include "engine/runtime/shutdown_signal.hpp"

#include <atomic>
#include <csignal>

namespace engine::runtime {

namespace {

std::atomic<bool> g_requested{false};

static_assert(std::atomic<bool>::is_always_lock_free, "le drapeau d'arret doit etre utilisable depuis un gestionnaire de signal");

extern "C" void onSignal(int) {
    g_requested.store(true);
}

}

foundation::Status ShutdownSignal::install() {
    if (std::signal(SIGINT, onSignal) == SIG_ERR || std::signal(SIGTERM, onSignal) == SIG_ERR) {
        return foundation::failStatus(foundation::ErrorCode::Io, "installation du gestionnaire de signal impossible");
    }
    return foundation::okStatus();
}

bool ShutdownSignal::requested() noexcept {
    return g_requested.load();
}

void ShutdownSignal::request() noexcept {
    g_requested.store(true);
}

void ShutdownSignal::reset() noexcept {
    g_requested.store(false);
}

}
