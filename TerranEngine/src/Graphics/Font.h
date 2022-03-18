#pragma once

//#include <freetype-gl.h>
#include "Texture.h"

#include "Core/Base.h"

#include <texture-atlas.h>
#include <texture-font.h>

namespace TerranEngine 
{
	class Font 
	{
	public:
		Font(const char* fontName, uint32_t fontSize);
		Shared<Texture> GetTexutre () const;

		ftgl::texture_glyph_t* LoadGlyph(const char& character);

		~Font();

	private:
		ftgl::texture_atlas_t* m_TextureAtlas;
		ftgl::texture_font_t* m_TexutreFont;
		Shared<Texture> m_Texture;
	};
}