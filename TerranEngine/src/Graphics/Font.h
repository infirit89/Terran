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

namespace msdfgen
{
	struct FontMetrics;
}

namespace TerranEngine 
{
	struct GlyphData final
	{
		glm::vec2 UVs[4];
		glm::vec4 VertexPositions[4];
		float Advance;
	};

	class Font final
	{
	public:
		Font(const std::string& fontPath);
		~Font();

		Shared<Texture2D> GetTexture() const { return m_Texture; }

		GlyphData GetGlyphData(char c) const;
		const msdfgen::FontMetrics& GetMetrics() const;

		double GetKerning(char c1, char c2) const;
		const std::string& GetPath() const { return m_Path; }

		// gets the advance between two characters with kerning
		double GetAdvance(char c1, char c2) const;

		static Shared<Font> DefaultFont;

	private:
		Shared<Texture2D> LoadFont(const std::string& fontPath);

	private:
		Shared<Texture2D> m_Texture;
		std::vector<msdf_atlas::GlyphGeometry>* m_Glyphs;
		msdf_atlas::FontGeometry* m_FontGeometry;
		int m_AtlasWidth, m_AtlasHeight;
		std::string m_Path;
	};
}