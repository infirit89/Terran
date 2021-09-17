#include "trpch.h"

#include "Font.h"

namespace TerranEngine 
{
	Font::Font(const char* fontName, uint32_t fontSize)
	{
		m_TextureAtlas = ftgl::texture_atlas_new((fontSize / 2) * 16, (fontSize / 2) * 16, 4);
		m_TexutreFont = ftgl::texture_font_new_from_file(m_TextureAtlas, fontSize, fontName);

		m_Texture = CreateShared<Texture>(m_TextureAtlas->width, m_TextureAtlas->height);
	}

	Shared<Texture> Font::GetTexutre() const
	{
		m_Texture->SetData(m_TextureAtlas->data, TextureType::RGBA);
		return m_Texture;
	}

	ftgl::texture_glyph_t* Font::LoadGlyph(const char& character)
	{
		std::string codepoint(1, character); 
		return ftgl::texture_font_get_glyph(m_TexutreFont, codepoint.c_str());
	}



	Font::~Font()
	{
		ftgl::texture_font_delete(m_TexutreFont);
		ftgl::texture_atlas_delete(m_TextureAtlas);
	}
}