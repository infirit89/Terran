#pragma once

#include "Log.h"

#if defined(TR_DEBUG)
	#define TR_ASSERT(condition, ...) if(!(condition)) { TR_ERROR(__VA_ARGS__); __debugbreak(); }
#elif defined(TR_RELEASE)
	#define TR_ASSERT
#endif
