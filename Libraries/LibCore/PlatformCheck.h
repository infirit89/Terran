#pragma once

#ifdef _WIN32
	#ifdef _WIN64
		#define TR_WINDOWS_64
	#else
		#error "Terran doesn't support win 32 platforms yet"
	#endif
#elif defined(__linux__)
	#define TR_LINUX
	#error "Terran doesn't support linux platforms yet"
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#define TR_APPLE
	#if TARGET_OS_OSX == 1
		#define TR_MACOSX
	#else
		#error "Unsupported Apple platform"
	#endif
#else
	#error "Unknown platform"
#endif
