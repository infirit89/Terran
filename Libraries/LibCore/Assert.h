#pragma once

#include "Log.h"
#include "PlatformCheck.h"

#if defined(TR_WINDOWS)
	#define TR_DEBUGBREAK __debugbreak
#elif defined(TR_MACOSX)
	#define TR_DEBUGBREAK __builtin_trap
#endif


#if defined(TR_CORE_DEBUG)
	#define TR_ASSERT(condition, ...) if(!(condition)) { TR_CORE_ERROR(TR_LOG_CORE, __VA_ARGS__); TR_DEBUGBREAK(); }
#elif defined(TR_RELEASE)
	#define TR_ASSERT
#endif
