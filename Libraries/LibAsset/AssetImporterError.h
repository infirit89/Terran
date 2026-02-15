#pragma once

#include "AssetError.h"

#include <cstdint>
#include <string_view>

namespace Terran::Asset {

class AssetImporterError final : public AssetError {
public:
    enum Code : uint8_t {
        ImporterNotFound,
    };

    AssetImporterError(Code code)
        : m_code(code)
    {
    }

    ~AssetImporterError() override = default;

    [[nodiscard]] std::string_view message() const override {
        switch(m_code) {
            case Code::ImporterNotFound: return "No importer registered for asset type";
        }
    }

    [[nodiscard]] std::string_view source() const override {
        return "Asset";
    }

private:
    Code m_code;
};

}
