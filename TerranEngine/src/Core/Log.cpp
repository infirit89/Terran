#include "trpch.h"
#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace TerranEngine 
{
	Shared<spdlog::logger> Log::s_CoreLogger;
	Shared<spdlog::logger> Log::s_ClientLogger;
	void Log::Init() 
	{
		//spdlog::set_pattern("%^[%T] %n: %v%$");
		
		std::vector<spdlog::sink_ptr> coreSinks
		{
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Terran.log", true)
		};

		coreSinks[0]->set_pattern("%^[%T] %n: %v%$");
		coreSinks[1]->set_pattern("%^[%T] %n: %v%$");

		s_CoreLogger = CreateShared<spdlog::logger>("TERRAN", coreSinks.begin(), coreSinks.end());
		s_CoreLogger->set_level(spdlog::level::trace);

		std::vector<spdlog::sink_ptr> clientSinks
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/TerranClient.log", true)
		};

		clientSinks[0]->set_pattern("%^[%T] %n: %v%$");
		s_ClientLogger = CreateShared<spdlog::logger>("TERRAN_CLIENT", clientSinks.begin(), clientSinks.end());
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}