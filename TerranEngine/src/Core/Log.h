#pragma once

#include "UUID.h"
#include "Base.h"

#include <glm/glm.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

namespace TerranEngine 
{
	class Log 
	{
	public:
		static void Init();
		static inline Shared<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
	private:
		static Shared<spdlog::logger> s_CoreLogger;

		// TODO: hook up to editor log
		static Shared<spdlog::logger> s_ClientLogger;
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

	template<typename OStream>
	OStream& operator<<(OStream& os, const glm::vec2& vec) 
	{
		return os << vec.x << ", " << vec.y;
	}

	template<typename OStream>
	OStream& operator<<(OStream& os, const glm::vec3& vec)
	{
		return os << vec.x << ", " << vec.y;
	}

	template<typename OStream>
	OStream& operator<<(OStream& os, const glm::vec4& vec) 
	{
		return os << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
	}

#define TR_TRACE(...) ::TerranEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TR_INFO(...) ::TerranEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TR_WARN(...) ::TerranEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TR_ERROR(...) ::TerranEngine::Log::GetCoreLogger()->error(__VA_ARGS__)

// TODO: client log macros
}
