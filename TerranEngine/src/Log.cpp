#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace TerranEngine {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

	void Log::Init()
	{
		s_CoreLogger = spdlog::stdout_color_mt("TERRAN");
		s_CoreLogger->set_level(spdlog::level::trace);
	}
}
