#pragma once

#include "BindingBase.h"

#include <LibCore/Base.h>
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
class LogBinding final {
public:
    static void bind(Coral::ManagedAssembly& assembly, Core::RawPtr<ScriptEngine> script_engine)
    {
        s_script_engine = script_engine;
        BIND_INTERNAL_FUNC(Log_LogICall);
    }
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
            s_script_engine->dispatch_log(messageStr, spdlog::level::trace);
            break;
        case ScriptLogLevel::Warn:
            TR_WARN(SCRIPT_SYSTEM, messageStr);
            s_script_engine->dispatch_log(messageStr, spdlog::level::warn);
            break;
        case ScriptLogLevel::Error:
            TR_ERROR(SCRIPT_SYSTEM, messageStr);
            s_script_engine->dispatch_log(messageStr, spdlog::level::err);
            break;
        default:;
        }
    }

    static Core::RawPtr<ScriptEngine> s_script_engine;
};


}
