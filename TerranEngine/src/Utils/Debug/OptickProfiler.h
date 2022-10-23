#pragma once

#include <optick.h>

#define TR_PROFILE_OPTICK 1
#if TR_PROFILE_OPTICK
	#define TR_START_CAPTURE()				OPTICK_START_CAPTURE()
	#define TR_STOP_CAPTURE(fileName)		OPTICK_STOP_CAPTURE(); OPTICK_SAVE_CAPTURE(fileName)
	#define TR_PROFILE_SCOPE(name)			OPTICK_EVENT(name)
	#define TR_PROFILE_FUNCTION()			OPTICK_EVENT()
	#define TR_PROFILE_FRAME(frameName)		OPTICK_FRAME(frameName)
#else
	#define TR_START_CAPTURE()
	#define TR_STOP_CAPTURE(fileName)
	#define TR_PROFILE_SCOPE(name)
	#define TR_PROFILE_FUNCTION()
	#define TR_PROFILER_FRAME(frameName) 
#endif