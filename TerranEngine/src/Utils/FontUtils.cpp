#include "trpch.h"
#include "FontUtils.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace TerranEngine 
{
	void FontUtils::LoadFont(const char* fontPath)
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
	}
}

