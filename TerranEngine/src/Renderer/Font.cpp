#include "trpch.h"

#include "Font.h"

namespace TerranEngine 
{
	Font::Font(const char* fontName, uint32_t fontSize)
	{
		m_TextureAtlas = ftgl::texture_atlas_new(512, 512, 1);
		m_TexutreFont = ftgl::texture_font_new_from_file(m_TextureAtlas, fontSize, fontName);

		m_Texture = new Texture(m_TextureAtlas->width, m_TextureAtlas->height);

		LoadGlyph('A');
	}

	Texture* Font::GetTexutre() const
	{
		m_Texture->SetData(m_TextureAtlas->data, TextureType::RED);
		return m_Texture;
	}

	void Font::LoadGlyph(const char character)
	{
		std::string codepoint = std::string(1, character);
		ftgl::texture_font_get_glyph(m_TexutreFont, codepoint.c_str());
	}

	Font::~Font()
	{
		delete m_Texture;
		ftgl::texture_font_delete(m_TexutreFont);
		ftgl::texture_atlas_delete(m_TextureAtlas);
	}
}