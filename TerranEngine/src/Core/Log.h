#pragma once

#include "UUID.h"
#include "Base.h"

#include <glm/glm.hpp>

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <memory>

namespace TerranEngine 
{
	class Log 
	{
	public:
		static void Init();
		static inline Shared<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
		
		static inline Shared<spdlog::logger> GetClientLogger() { return s_ClientLogger; }
		static inline void SetClientLogger(Shared<spdlog::logger> logger) { s_ClientLogger = logger; }
	private:
		static Shared<spdlog::logger> s_CoreLogger;

		// TODO: hook up to editor log
		static Shared<spdlog::logger> s_ClientLogger;
	};

	template<typename OStream>
	OStream& operator<<(OStream& os, const UUID& uuid)
	{
		std::array<uint8_t, 16> idArr = id.GetData() = uuid.GetData();

		int ind = 0;
		for (int i = 0; i < 20; i++)
		{
			if (i == 4 || i == 7 || i == 10 || i == 13)
			{
				os << '-';
				continue;
			}

			os << std::hex
				<< (int)(idArr[ind] >> 4 & 0x0f)
				<< (int)(idArr[ind] & 0x0f);

			ind++;
		}

		return os;
	}

	template<typename OStream>
	OStream& operator<<(OStream& os, const glm::vec2& vec) 
	{
		os << vec.x << ", " << vec.y;
		return os;
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
#define TR_CLIENT_TRACE(...) ::TerranEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TR_CLIENT_INFO(...) ::TerranEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define TR_CLIENT_WARN(...) ::TerranEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TR_CLIENT_ERROR(...) ::TerranEngine::Log::GetClientLogger()->error(__VA_ARGS__)

}
