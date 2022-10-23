#pragma once

#include "Core/Log.h"

#include <chrono>
#include <vector>

namespace TerranEngine 
{
	struct ProfileResult 
	{
		std::string Name;
		double ElapsedTime;
	};

	class Profiler 
	{
	public:

		Profiler() {}
		~Profiler() {}

		std::vector<ProfileResult>& GetResults() 
		{
			return m_Results;
		}

		void ClearResults() 
		{
			m_Results.clear();
		}

		static Profiler& Get() 
		{
			static Profiler instance;
			return instance;
		}
	private:
		std::vector<ProfileResult> m_Results;
		friend class ProfileTimer;
	};

	class ProfileTimer 
	{
	public:
		ProfileTimer(const char* name)
			: m_Name(name) 
		{
			m_StartPoint = std::chrono::high_resolution_clock::now();
		}

		~ProfileTimer() 
		{
			Stop();
		}

	private:
		void Stop() 
		{
			auto m_EndPoint = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndPoint).time_since_epoch().count();

			double millis = (end - start) * 0.001;

			ProfileResult result = { m_Name, millis };

			Profiler::Get().m_Results.emplace_back(result);
		}

	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartPoint;
	};
}

#define TR_PROFILE_INTERNAL 0
#if TR_PROFILE_INTERNAL
	#define TR_PROFILE_FUNCN(name)	::TerranEngine::ProfileTimer timer##__LINE__(name)
	#define TR_PROFILE_FUNC()		TR_PROFILE_FUNCN(__func__)
#else
	#define TR_PROFILE_FUNCN(name)
	#define TR_PROFILE_FUNC()
#endif