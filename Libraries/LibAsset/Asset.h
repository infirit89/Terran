#pragma once

#include "AssetTypes.h"

#include <LibCore/Hash.h>

#include <concepts>
#include <string>

namespace Terran {
namespace Asset {

class Asset {
public:
    Asset() = default;
    virtual ~Asset() = default;
    Asset(AssetHandle const& handle)
        : m_handle(handle)
    {
    }

    bool is_valid()
    {
        return m_handle.is_valid();
    }

    AssetHandle const& handle()
    {
        return m_handle;
    }

    virtual AssetTypeId type() const = 0;

protected:
    AssetHandle m_handle;
    AssetTypeId m_type_id;
    friend class AssetManager;
};

template <typename T>
concept HasStaticType = requires(T) {
    { T::static_type() } -> std::same_as<AssetTypeId>;
}; 

#define TR_DECLARE_ASSET_TYPE(type_name)                                                  \
    static AssetTypeId static_type() { return Terran::Core::Hash::fnv1a_64(#type_name); } \
    virtual AssetTypeId type() const override { return static_type(); }

class TextAsset final : public Asset {
public:
    TextAsset(std::string const& text)
        : m_text(text)
    {
    }
    ~TextAsset() override = default;

    std::string const& text() { return m_text; }
    TR_DECLARE_ASSET_TYPE(TextAsset);

private:
    std::string m_text;
};

}

}
