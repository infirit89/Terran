#pragma once

#include "Core/Log.h"

#include <chrono>

namespace TerranEngine 
{
	class DebugTimer 
	{
	public:
		DebugTimer() 
		{
			m_StartPoint = std::chrono::high_resolution_clock::now();
		}
		~DebugTimer()
		{
			auto m_EndPoint = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartPoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndPoint).time_since_epoch().count();

			auto milis = (end - start) * 0.001;

			TR_TRACE("Microseconds: {0}; Miliseconds {1}", end - start, milis);
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartPoint;
	};
}