#pragma once

#include "BindingBase.h"

#include <LibCore/Base.h>
#include <LibCore/UUID.h>

#include <LibScene/Components.h>
#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>

namespace Terran::Script {

class TagComponentBindings final {
public:
    static void bind(
        Coral::ManagedAssembly& assembly,
        Core::RawPtr<World::SceneManager> scene_manager)
    {
        s_scene_manager = scene_manager;
        TR_BIND_INTERNAL_FUNC(Tag_GetNameICall);
        TR_BIND_INTERNAL_FUNC(Tag_SetNameICall);
    }

private:
    static Coral::String Tag_GetNameICall(Core::UUID const& id)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return Coral::String::New("");

        return Coral::String::New(entity.name());
    }

    static void Tag_SetNameICall(Core::UUID const& id, Coral::String name)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return;

        entity.get_component<World::TagComponent>().Name = name;
    }

    inline static Core::RawPtr<World::SceneManager> s_scene_manager = nullptr;
};

}
