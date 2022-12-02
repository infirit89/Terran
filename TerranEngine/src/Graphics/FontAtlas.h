#pragma once

#include "Texture.h"
#include "Core/Base.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>

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

		double GetKerning(char c1, char c2);
		std::string GetPath() { return m_Path; }

	private:
		Shared<Texture> LoadFont(const std::string& fontPath);

	private:
		Shared<Texture> m_Texture;
		std::vector<msdf_atlas::GlyphGeometry>* m_Glyphs;
		msdf_atlas::FontGeometry* m_FontGeometry;
		int m_AtlasWidth, m_AtlasHeight;
		std::string m_Path;
	};
}