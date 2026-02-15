#pragma once

#include <string_view>

namespace Terran::Asset {

class AssetError {
public:
    virtual ~AssetError() = default;
    [[nodiscard]] virtual std::string_view message() const = 0;
    [[nodiscard]] virtual std::string_view source() const = 0;
};

}
