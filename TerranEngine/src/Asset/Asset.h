#pragma once
#include "LibCore/UUID.h"

#include <filesystem>

namespace TerranEngine {

enum class AssetType {
    None = 0,
    Texture2D,
    Text,
    ScriptFile,
    Scene,
    PhysicsMaterial2D
};

typedef Terran::Core::UUID AssetHandle;

struct AssetInfo {
    AssetInfo() = default;
    ~AssetInfo() = default;

    bool operator==(AssetInfo const& other) const { return Path == other.Path && Type == other.Type; }
    bool operator!=(AssetInfo const& other) const { return !((*this) == other); }
    operator bool() const { return Path != "" && Type != AssetType::None; }

    std::filesystem::path Path = "";
    AssetType Type = AssetType::None;
    AssetHandle Handle = AssetHandle::Invalid();
};

class Asset {
public:
    Asset() = default;
    virtual ~Asset() = default;
    Asset(AssetHandle const& handle)
        : m_Handle(handle)
    {
    }

    bool IsValid() { return m_Handle.IsValid(); }
    AssetHandle const& GetHandle() { return m_Handle; }
    virtual AssetType GetType() const = 0;

protected:
    AssetHandle m_Handle;
    friend class AssetManager;
};

#define ASSET_CLASS_TYPE(type)                                   \
    static AssetType GetStaticType() { return AssetType::type; } \
    virtual AssetType GetType() const override { return GetStaticType(); }

class TextAsset final : public Asset {
public:
    TextAsset(std::string const& text)
        : m_Text(text)
    {
    }
    ~TextAsset() override = default;

    std::string const& GetText() { return m_Text; }

    ASSET_CLASS_TYPE(Text)

private:
    std::string m_Text;
};

}
