#pragma once

#include "AssetManager.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>

namespace Terran::Asset {

class AssetSystem final : public Core::Layer {

public:
    AssetSystem(Core::EventDispatcher& dispatcher)
        : Core::Layer("Asset", dispatcher)
    {
        m_asset_manager = Core::CreateShared<AssetManager>(dispatcher);
    }

    Core::Shared<AssetManager> asset_manager() const
    {
        return m_asset_manager;
    }

private:
    Core::Shared<AssetManager> m_asset_manager;
};

}
