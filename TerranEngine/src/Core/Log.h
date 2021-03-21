#pragma once

#include <memory>
#include "spdlog/spdlog.h"

namespace TerranEngine 
{
	class Log 
	{
	public:
		static void Init();
		static inline std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
#define TR_CORE_TRACE(...) ::TerranEngine::Log::GetCoreLogger()->trace(__VA_ARGS_)
#define TR_CORE_INFO(...) ::TerranEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TR_CORE_WARN(...) ::TerranEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TR_CORE_ERROR(...) ::TerranEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
}