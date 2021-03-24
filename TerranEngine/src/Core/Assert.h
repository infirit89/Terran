#pragma once

#include "Log.h"

#if defined(TR_DEBUG)
	#define TR_ASSERTMSG(condition, ...) if(!(condition)) { TR_ERROR(__VA_ARGS__); __debugbreak(); } 
	#define TR_ASSERT(condition) if(!(condition)) __debugbreak()
#elif defined(TR_RELEASE)
	#define TR_ASSERTMSG
	#define TR_ASSERT
#endif
