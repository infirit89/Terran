#pragma once

#include "Log.h"
#include "PlatformCheck.h"

#ifdef TR_DEBUG
	#if defined(TR_WINDOWS_64)
		#define TR_DEBUGBREAK() __debugbreak()
	#elif defined(TR_LINUX)
		#include <signal.h>
		#define TR_DEBUGBREAK() raise(SIGTRAP)
	#endif
#endif

#if defined(TR_DEBUG)
	#define TR_ASSERT(condition, ...)	\
	do									\
	{									\
		if(!(condition))				\
		{								\
			TR_ERROR(__VA_ARGS__);		\
			TR_DEBUGBREAK();			\
		}								\
	} while(false)

#elif defined(TR_RELEASE)
	#define TR_ASSERT
#endif
