#pragma once

#include <string_view>
#include <cstdint>

namespace TerranEngine 
{
	enum class PhysicsBodyType : uint8_t
	{
		Static = 0,
		Dynamic,
		Kinematic
	};

	std::string_view PhysicsBodyTypeToString(PhysicsBodyType bodyType);
	PhysicsBodyType PhysicsBodyTypeFromString(const std::string& bodyTypeString);

	enum class PhysicsBodySleepState : uint8_t
	{
		Sleep = 0,
		Awake,
		NeverSleep
	};

	std::string_view PhysicsBodySleepStateToString(PhysicsBodySleepState sleepState);
	PhysicsBodySleepState PhysicsBodySleepStateFromString(const std::string& sleepStateString);

	enum class ForceMode2D : uint8_t
	{
		Force = 0,
		Impulse
	};
}
