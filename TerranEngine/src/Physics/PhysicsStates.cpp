#include "trpch.h"
#include "PhysicsStates.h"

namespace TerranEngine 
{
	std::string PhysicsBodyTypeToString(PhysicsBodyType bodyType)
	{
		switch (bodyType)
		{
		case PhysicsBodyType::Static:		return "Static";
		case PhysicsBodyType::Dynamic:		return "Dynamic";
		case PhysicsBodyType::Kinematic:	return "Kinematic";
		}

		TR_ASSERT(false, "Unknown body type");
		return "";
	}

	PhysicsBodyType PhysicsBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")		return PhysicsBodyType::Static;
		if (bodyTypeString == "Dynamic")	return PhysicsBodyType::Dynamic;
		if (bodyTypeString == "Kinematic")	return PhysicsBodyType::Kinematic;

		TR_ASSERT(false, "Unknown body type");
		return PhysicsBodyType::Static;
	}

	std::string PhysicsBodySleepStateToString(PhysicsBodySleepState sleepState)
	{
		switch (sleepState)
		{
		case PhysicsBodySleepState::Sleep:		return "Sleep";
		case PhysicsBodySleepState::Awake:		return "Awake";
		case PhysicsBodySleepState::NeverSleep: return "NeverSleep";
		}

		TR_ASSERT(false, "Unkown sleep state");
		return "";
	}

	PhysicsBodySleepState PhysicsBodySleepStateFromString(const std::string& sleepStateString)
	{
		if (sleepStateString == "Sleep") return PhysicsBodySleepState::Sleep;
		if (sleepStateString == "Awake") return PhysicsBodySleepState::Awake;
		if (sleepStateString == "NeverSleep") return PhysicsBodySleepState::NeverSleep;

		TR_ASSERT(false, "Unknown sleep state");
		return PhysicsBodySleepState::Sleep;
	}
}
