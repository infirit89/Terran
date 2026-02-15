#pragma once

#include "AssetError.h"

#include <cstdint>
#include <string_view>

namespace Terran::Asset {

class AssetImporterError final : public AssetError {
public:
    enum class Code : uint8_t {
        ImporterNotFound,
    };

    explicit AssetImporterError(Code code)
        : m_code(code)
    {
    }

    ~AssetImporterError() override = default;

    [[nodiscard]] std::string_view message() const override
    {
        if (m_code == Code::ImporterNotFound) {
            return "No importer registered for asset type";
        }

        return "Unknown error";
    }

    [[nodiscard]] std::string_view source() const override
    {
        return "Asset";
    }

private:
    Code m_code;
};

}
