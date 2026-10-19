#pragma once

#include "engine/resources/api/iresource_provider.hpp"

namespace engine::resources {

class NullResourceProvider final : public IResourceProvider {
public:
    [[nodiscard]] bool exists(std::string_view id) const override;
    [[nodiscard]] foundation::Result<std::vector<std::byte>> load(std::string_view id) override;
};

}
