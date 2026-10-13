#pragma once

#include "BindingBase.h"

#include <vector>
#include <Coral/Assembly.hpp>
#include <LibCore/Log.h>
#include <LibScript/ScriptEngine.h>
#include <LibScript/ScriptTypes.h>
#include <cstdint>
#include <spdlog/common.h>
#include <string>

namespace Terran::Script {

// NOTE: maybe pass the ScriptEngine instance to C# as an IntPtr and then call internal functions with that instance
class LogBinding final : public BindingBase {
public:
    LogBinding(Coral::ManagedAssembly& assembly)
    {
        BIND_INTERNAL_FUNC(LogBinding::Log_LogICall);
    }
    virtual ~LogBinding() override = default;

private:
    enum class ScriptLogLevel : uint8_t {
        Trace = 1 << 0,
        Warn = 1 << 1,
        Error = 1 << 2,
    };

    static void Log_LogICall(uint8_t logLevel, Coral::String message)
    {
        // TR_PROFILE_FUNCTION();
        std::string messageStr = message;
        switch ((ScriptLogLevel)logLevel) {
        case ScriptLogLevel::Trace:
            TR_TRACE(SCRIPT_SYSTEM, messageStr);
            ScriptEngine::the()->DispatchLog(messageStr, spdlog::level::trace);
            break;
        case ScriptLogLevel::Warn:
            TR_WARN(SCRIPT_SYSTEM, messageStr);
            ScriptEngine::the()->DispatchLog(messageStr, spdlog::level::warn);
            break;
        case ScriptLogLevel::Error:
            TR_ERROR(SCRIPT_SYSTEM, messageStr);
            ScriptEngine::the()->DispatchLog(messageStr, spdlog::level::err);
            break;
        default:;
        }
    }
};

}
