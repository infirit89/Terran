#pragma once

#include <cstdint>

namespace TerranEngine 
{
	enum class PhysicsBodyType : uint8_t
	{
		Static = 0,
		Dynamic,
		Kinematic
	};

	enum class PhysicsBodySleepState : uint8_t
	{
		Sleep = 0,
		Awake,
		NeverSleep
	};

	enum class ForceMode2D : uint8_t
	{
		Force = 0,
		Impulse
	};

}