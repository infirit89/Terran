#pragma once

#include <LibScript/bindings/BindingBase.h>
#include <vector>
#include <Coral/Assembly.hpp>
#include <LibCore/Log.h>
#include <LibScript/ScriptEngine.h>
#include <LibScript/ScriptTypes.h>
#include <cstdint>
#include <spdlog/common.h>
#include <string>

namespace Terran::Script {

class SceneBidning final : public BindingBase {

public:
    SceneBidning(Coral::ManagedAssembly& assembly)
    {
        // BIND_INTERNAL_FUNC(LogBinding::Log_LogICall);
    }
    virtual ~SceneBidning() override = default;

private:
    // static bool SceneManager_LoadSceneICall(Coral::String scenePath);
};

}
