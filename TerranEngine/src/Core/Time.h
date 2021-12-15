#pragma once

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

	private:
		float m_Time = 0.0f;
	};
}