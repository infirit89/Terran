#pragma once

#include "DataStructures.h"
#include "Renderer/Texture.h"

namespace TerranEngine 
{
	class FontUtils 
	{
	public:
		static Texture& LoadFont(const char* fontPath/*, std::vector<Character>& characters*/);
	};
}