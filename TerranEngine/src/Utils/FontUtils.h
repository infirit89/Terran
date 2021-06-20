#pragma once

#include "DataStructures.h"

#include <map>

namespace TerranEngine 
{
	class FontUtils 
	{
	public:
		static void LoadFont(const char* fontPath, std::vector<Character>);
	};
}