#pragma once

#include "AssetTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Hash.h>

#include <LibCore/RefPtr.h>
#include <concepts>
#include <string>

namespace Terran {
namespace Asset {

class AssetManager;
class Asset : public Core::RefCounted {
public:
    Asset() = default;
    Asset(AssetId const& handle)
        : m_id(handle)
    {
    }

    bool is_valid()
    {
        return m_id.is_valid();
    }

    AssetId const& id()
    {
        return m_id;
    }

    virtual AssetTypeId type() const = 0;

private:
    AssetId m_id;
    AssetTypeId m_type_id;
    friend class AssetManager;
};

template <typename T>
concept HasStaticType = requires(T) {
    { T::static_type() } -> std::same_as<AssetTypeId>;
}; 

#define TR_DECLARE_ASSET_TYPE(type_name)                                                  \
    static Terran::Asset::AssetTypeId static_type() { return Terran::Core::Hash::fnv1a_64(#type_name); } \
    virtual Terran::Asset::AssetTypeId type() const override { return static_type(); }

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
