#pragma once

#include <ctime>

namespace TerranEngine 
{
	class Time 
	{
	public:
		Time(float time) 
			: m_Time(time)
		{}

		float GetDeltaTimeMS() { return m_Time * 1000.0f; }
		float GetDeltaTime() { return m_Time; }
		
		static std::tm* GetCurrentTime() 
		{
			std::time_t time = std::time(nullptr);
			return std::localtime(&time);
		}

	private:
		float m_Time = 0.0f;
	};
}