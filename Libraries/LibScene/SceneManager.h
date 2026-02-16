#pragma once

#include "Scene.h"

#include <LibAsset/AssetSystem.h>
#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/UUID.h>

namespace Terran::World {

class SceneManager final : public Core::Layer {

public:
    SceneManager(Core::EventDispatcher& dispatcher, Core::RawPtr<Asset::AssetSystem> asset_system)
        : Core::Layer("Scene", dispatcher)
        , m_asset_system(asset_system)
    {
    }

    Core::Shared<Scene> create_empty_scene();

    Core::Shared<Scene> const& current_scene()
    {
        return m_current_scene;
    }

    void reset_current_scene()
    {
        m_current_scene.reset();
    }

    void set_current_scene(Core::Shared<Scene> new_scene);
    Core::Shared<Scene> copy_scene(Core::Shared<Scene> const& source_scene);

private:
    Core::Shared<Scene> m_current_scene;
    Core::RawPtr<Asset::AssetSystem> m_asset_system;
};

}
