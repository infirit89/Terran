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
	#define TR_APPLE	// TODO: Split into apple's different platforms
	#error "Terran doesn't support apple platforms yet"
#else
	#error "Unknown platform"
#endif