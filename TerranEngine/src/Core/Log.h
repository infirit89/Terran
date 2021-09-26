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
	os << std::hex 
		<< (int)uuid.m_Data[0]
		<< (int)uuid.m_Data[1]
		<< (int)uuid.m_Data[2]
		<< (int)uuid.m_Data[3]
		<< '-'
		<< (int)uuid.m_Data[4]
		<< (int)uuid.m_Data[5]
		<< '-'
		<< (int)uuid.m_Data[6]
		<< (int)uuid.m_Data[7]
		<< '-'
		<< (int)uuid.m_Data[8]
		<< (int)uuid.m_Data[9]
		<< '-'
		<< (int)uuid.m_Data[10]
		<< (int)uuid.m_Data[11]
		<< (int)uuid.m_Data[12]
		<< (int)uuid.m_Data[13]
		<< (int)uuid.m_Data[14]
		<< (int)uuid.m_Data[15];

	return os;
}

#define TR_TRACE(...) ::TerranEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TR_INFO(...) ::TerranEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TR_WARN(...) ::TerranEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TR_ERROR(...) ::TerranEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
}