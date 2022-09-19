#pragma once

#include "UUID.h"
#include "Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) 
	{
		return ctx.end();
	}

	template<typename FormatContext>
	auto format(const glm::vec<1, T, Q>& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", vec[0]);
	}

	template<typename FormatContext>
	auto format(const glm::vec<2, T, Q>& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}, {}", vec[0], vec[1]);
	}

	template<typename FormatContext>
	auto format(const glm::vec<3, T, Q>& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}, {}, {}", vec[0], vec[1], vec[2]);
	}

	template<typename FormatContext>
	auto format(const glm::vec<4, T, Q>& vec, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}, {}, {}, {}", vec[0], vec[1], vec[2], vec[3]);
	}
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> 
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.end();
	}
	
	template<typename FormatContext>
	auto format(const glm::qua<T, Q>& qua, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}, {}, {}, {}", qua.x, qua.y, qua.z, qua.w);
	}
};

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
		static Shared<spdlog::logger> s_ClientLogger;
	};

	template<typename OStream>
	OStream& operator<<(OStream& os, const UUID& uuid)
	{
		std::array<uint8_t, 16> idArr = uuid.GetData();

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
