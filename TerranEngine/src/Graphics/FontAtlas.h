#pragma once

#include "Texture.h"
#include "Core/Base.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace msdf_atlas 
{
	class GlyphGeometry;
	class FontGeometry;
}

namespace TerranEngine 
{
	struct GlyphData 
	{
		glm::vec2 UVs[4];
		glm::vec4 VertexPositions[4];
		float Advance;
	};

	class FontAtlas 
	{
	public:

		FontAtlas();
		FontAtlas(const std::string& fontPath);
		~FontAtlas();

		inline Shared<Texture> GetTexture() const { return m_Texture; }

		GlyphData GetGlyphData(char c);

	private:
		Shared<Texture> LoadFont(const std::string& fontPath);

	private:
		Shared<Texture> m_Texture;
		std::vector<msdf_atlas::GlyphGeometry>* m_Glyphs;
		msdf_atlas::FontGeometry* m_FontGeometry;
		int m_AtlasWidth, m_AtlasHeight;
	};
}