#pragma once

namespace TerranEngine 
{

	class Time 
	{
	public:
		Time(float time) 
			: m_Time(time) {}

		float GetSeconds() { return m_Time; }
		float GetMiliSeconds() { return m_Time / 1000; }
	private:
		float m_Time;
	};
}