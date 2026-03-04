#pragma once
#include "AssetTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/RefPtr.h>
#include <LibCore/UUID.h>
#include <LibCore/RefCounted.h>
#include <LibCore/WeakRef.h>

namespace Terran::Asset {

class AssetManager;
class AssetHandle : public Core::RefCounted {
public:
    virtual ~AssetHandle() override;

    constexpr AssetId const& id() const
    {
        return m_id;
    }

    bool is_valid() const;
    explicit(false) constexpr operator bool() const {
        return is_valid();
    }

private:
    // A nifty trick to ensure AssetHandle is only used with RefPtr or WeakPtr
    AssetHandle(AssetId const& id, Core::RawPtr<AssetManager const> asset_manager)
        : m_id(id)
        , m_asset_manager(asset_manager)
    {
    }

private:
    AssetId m_id;
    Core::RawPtr<AssetManager const> m_asset_manager;
    template<Core::IsRefCounted T>
    friend class Core::RefPtr;

    template<Core::IsRefCounted T>
    friend class Core::WeakPtr;
};

using StrongAssetHandle = Core::RefPtr<AssetHandle>;
using WeakAssetHandle = Core::WeakPtr<AssetHandle>;

}

