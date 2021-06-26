#include "trpch.h"
#include "FontUtils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

namespace TerranEngine 
{
	Texture& FontUtils::LoadFont(const char* fontPath/*, std::vector<character>& characters*/)
	{
		FT_Library ft;

		if (FT_Init_FreeType(&ft)) 
		{
			TR_ERROR("Couldn't initialize freetype");
			return Texture::EmptyTexture();
		}

		FT_Face face;

		if (FT_New_Face(ft, fontPath, 0, &face)) 
		{
			TR_ERROR("Couldn't load font");
			return Texture::EmptyTexture();
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		uint32_t fontWidth = 0, fontHeight = 0;

		uint16_t** fontData = new uint16_t*[128];

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				TR_ERROR("Failed to load glyph");
				continue;
			}

			fontWidth += face->glyph->bitmap.width;
			fontHeight += face->glyph->bitmap.rows;

			Character character =
			{
				c,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			if(face->glyph->bitmap.buffer != NULL)
				fontData[c] = face->glyph->bitmap.buffer;
			//characters.emplace_back(character);
		}
		
		Texture tex(fontWidth, fontHeight);
		tex.SetData(fontData, GL_RED, GL_RED);
		
		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		delete[] fontData;

		return tex;
	}
}
