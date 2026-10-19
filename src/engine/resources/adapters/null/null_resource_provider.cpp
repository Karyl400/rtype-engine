#include "engine/resources/adapters/null/null_resource_provider.hpp"

#include <string>

namespace engine::resources {

bool NullResourceProvider::exists(std::string_view) const {
    return false;
}

foundation::Result<std::vector<std::byte>> NullResourceProvider::load(std::string_view id) {
    return foundation::Result<std::vector<std::byte>>::fail(
        foundation::Error{foundation::ErrorCode::NotFound, "ressource introuvable : " + std::string{id}});
}

}
