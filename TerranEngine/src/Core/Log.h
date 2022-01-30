#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>
#include "UUID.h"

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

	template<typename OStream>
	OStream& operator<<(OStream& os, const UUID& uuid)
	{
		int ind = 0;
		for (int i = 0; i < 20; i++)
		{
			if (i == 4 || i == 7 || i == 10 || i == 13)
			{
				os << '-';
				continue;
			}

			os << std::hex
				<< (int)(uuid.m_Data[ind] >> 4 & 0x0f)
				<< (int)(uuid.m_Data[ind] & 0x0f);

			ind++;
		}

		return os;
	}

#define TR_TRACE(...) ::TerranEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TR_INFO(...) ::TerranEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TR_WARN(...) ::TerranEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TR_ERROR(...) ::TerranEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
}
