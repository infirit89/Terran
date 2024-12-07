#pragma once

#include <ctime>

namespace TerranEngine 
{
	class Time final
	{
	public:
		Time(float time) 
			: m_Time(time)
		{}

		float GetDeltaTimeMilliseconds() const { return m_Time * 1000.0f; }
		float GetDeltaTime() const { return m_Time; }
		
		static std::tm* GetCurrentTime() 
		{
			std::time_t time = std::time(nullptr);
			return std::localtime(&time);
		}

	private:
		float m_Time = 0.0f;
	};
}