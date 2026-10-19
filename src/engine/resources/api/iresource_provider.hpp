#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

#include "engine/foundation/noncopyable.hpp"
#include "engine/foundation/result.hpp"

namespace engine::resources {

class IResourceProvider : private foundation::NonCopyable {
public:
    virtual ~IResourceProvider();
    [[nodiscard]] virtual bool exists(std::string_view id) const = 0;
    [[nodiscard]] virtual foundation::Result<std::vector<std::byte>> load(std::string_view id) = 0;

protected:
    IResourceProvider();
};

}
