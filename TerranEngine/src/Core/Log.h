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

#include <unordered_map>

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

#define TR_LOG_CLIENT "Client"
#define TR_LOG_CORE "Core"
#define TR_LOG_RENDERER "Renderer"
#define TR_LOG_SCRIPT "Script"
#define TR_LOG_ASSET "Asset"
#define TR_LOG_PHYSICS "Physics"
#define TR_CORE_LOGGER_COUNT 6

struct LogSettings
{
	std::string Name;
	spdlog::level::level_enum Level;
};

namespace TerranEngine 
{
	class Log 
	{
	public:
		static void Init();
		static void Shutdown();
		//static void SetClientLogger(Shared<spdlog::logger> logger);
		static std::string GetFormattedFileLoggerName(std::string_view loggerName);
		static bool Contains(const std::string& loggerName);
		static Shared<spdlog::logger> GetLogger(const std::string& loggerName);
		static void SetLoggerSink(const std::string& loggerName, spdlog::sink_ptr sink, size_t sinkIndex = 0);
		static void SetLogFormat(const std::string& loggerName, const std::string& format);
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

	#define TR_CORE_TRACE(LoggerName, ...)								\
		if(TerranEngine::Log::Contains(LoggerName))						\
			TerranEngine::Log::GetLogger(LoggerName)->trace(__VA_ARGS__)

	#define TR_CORE_INFO(LoggerName, ...)								\
		if(TerranEngine::Log::Contains(LoggerName))						\
			TerranEngine::Log::GetLogger(LoggerName)->info(__VA_ARGS__)

	#define TR_CORE_WARN(LoggerName, ...)								\
		if(TerranEngine::Log::Contains(LoggerName))						\
			TerranEngine::Log::GetLogger(LoggerName)->warn(__VA_ARGS__)

	#define TR_CORE_ERROR(LoggerName, ...)								\
		if(TerranEngine::Log::Contains(LoggerName))						\
			TerranEngine::Log::GetLogger(LoggerName)->error(__VA_ARGS__)


	#define TR_CLIENT_TRACE(...)											\
		if(TerranEngine::Log::Contains(TR_LOG_CLIENT))						\
			TerranEngine::Log::GetLogger(TR_LOG_CLIENT)->trace(__VA_ARGS__)

	#define TR_CLIENT_INFO(...)												\
		if (TerranEngine::Log::Contains(TR_LOG_CLIENT))						\
			TerranEngine::Log::GetLogger(TR_LOG_CLIENT)->info(__VA_ARGS__)

	#define TR_CLIENT_WARN(...)												\
		if (TerranEngine::Log::Contains(TR_LOG_CLIENT))						\
			TerranEngine::Log::GetLogger(TR_LOG_CLIENT)->warn(__VA_ARGS__)

	#define TR_CLIENT_ERROR(...)											\
		if (TerranEngine::Log::Contains(TR_LOG_CLIENT))						\
			TerranEngine::Log::GetLogger(TR_LOG_CLIENT)->error(__VA_ARGS__)
}
