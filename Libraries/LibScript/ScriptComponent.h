#pragma once

#include <LibCore/UUID.h>

#include <string>
#include <vector>
#include <cstdint>

namespace Terran::Script {

struct ScriptComponent {
    ScriptComponent() = default;

    ScriptComponent(std::string const& moduleName)
        : ModuleName(moduleName)
    {
    }

    // NOTE: think about having an array of scripts so that one entity
    // "can" have more than one script (because of the 1 component of a type per entity)

    std::string ModuleName;
    std::vector<int32_t> FieldHandles;

    Core::UUID ScriptSourceHandle = Core::UUID::invalid();
    bool ClassExists = true;
};

}
