#include "trpch.h"
#include "FontUtils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

namespace TerranEngine 
{
	void FontUtils::LoadFont(const char* fontPath, std::vector<Character> characters)
	{
		FT_Library ft;

		if (FT_Init_FreeType(&ft)) 
		{
			TR_ERROR("Couldn't initialize freetype");
			return;
		}

		FT_Face face;

		if (FT_New_Face(ft, fontPath, 0, &face)) 
		{
			TR_ERROR("Couldn't load font");
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, 48);

		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) 
		{
			TR_ERROR("Failed to load Glyph");
			return;
		}

		TR_TRACE(face->num_glyphs);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
			{
				TR_ERROR("Failed to load glyph");
				continue;
			}


		}
	}
}

