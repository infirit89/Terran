#pragma once

#include <freetype-gl.h>
#include "Texture.h"

namespace TerranEngine 
{
	class Font 
	{
	public:
		Font(const char* fontName, uint32_t fontSize);
		Texture* GetTexutre () const;
		~Font();

	private:
		ftgl::texture_atlas_t* m_TextureAtlas;
		ftgl::texture_font_t* m_TexutreFont;
		Texture* m_Texture;
	};
}