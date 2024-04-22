#pragma once

#include "Log.h"

#if defined(TR_DEBUG)
	#define TR_ASSERT(condition, ...) if(!(condition)) { TR_CORE_ERROR(TR_LOG_CORE, __VA_ARGS__); __debugbreak(); }
#elif defined(TR_RELEASE)
	#define TR_ASSERT
#endif
