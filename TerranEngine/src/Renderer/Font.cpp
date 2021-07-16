#include "trpch.h"
#include "Font.h"

#include <freetype-gl.h>

namespace TerranEngine 
{
	Font::Font(const char* fontName, uint32_t fontSize)
	{
		m_TextureAtlas = ftgl::texture_atlas_new(512, 512, 1);
		m_TexutreFont = ftgl::texture_font_new_from_file(m_TextureAtlas, fontSize, fontName);

		m_Texture = new Texture(m_TextureAtlas->width, m_TexutreFont->height);
		m_Texture->SetData(m_TextureAtlas->data);
	}

	Texture* Font::GetTexutre() const
	{
		return m_Texture;
	}
	Font::~Font()
	{
		delete m_Texture;
		ftgl::texture_font_delete(m_TexutreFont);
		ftgl::texture_atlas_delete(m_TextureAtlas);
	}
}