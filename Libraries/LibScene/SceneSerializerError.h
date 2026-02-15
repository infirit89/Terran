#pragma once

#include <LibAsset/AssetError.h>

#include <cstdint>
#include <string_view>
namespace Terran::World {

class SceneSerializerError final : public Asset::AssetError {
public:
    enum Code : uint8_t {
        InvalidFormat,
        NotFound,
    };

    SceneSerializerError(Code code, std::string_view details = "")
        : m_code(code)
        , m_details(details)
    {
    }

    ~SceneSerializerError() override = default;

    [[nodiscard]] std::string_view message() const override
    {
        switch (m_code) {
        case InvalidFormat:
            return "Scene file has invalid format";
        case NotFound:
            return "Scene file was not found";
        }
    }

    [[nodiscard]] std::string_view source() const override
    {
        return m_details;
    }

private:
    Code m_code;
    std::string_view m_details;
};

}
