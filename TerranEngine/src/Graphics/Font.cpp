#include "trpch.h"
#include "Font.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#define MSDF_ATLAS_DEFAULT_EM_SIZE 32.0

namespace TerranEngine 
{
	Shared<Font> Font::DefaultFont;

	Font::Font(const std::string& fontPath)
		: m_Glyphs(nullptr), m_FontGeometry(nullptr), m_AtlasWidth(-1), m_AtlasHeight(-1), m_Path(fontPath)
	{
		m_Texture = LoadFont(fontPath);
	}

	Font::~Font()
	{
		if (m_Glyphs)
			delete m_Glyphs;

		if (m_FontGeometry)
			delete m_FontGeometry;
	}

	GlyphData Font::GetGlyphData(char c) const
	{
		const msdf_atlas::GlyphGeometry* glyph = m_FontGeometry->getGlyph(c);

		if (!glyph)
			glyph = m_FontGeometry->getGlyph('?');

		// couldn't even find a '?' what font is that
		if (!glyph)
			return {};

		double ul, vb, ur, vt;
		glyph->getQuadAtlasBounds(ul, vb, ur, vt);
		GlyphData glyphData;

		ul /= m_AtlasWidth;
		vb /= m_AtlasHeight;
		ur /= m_AtlasWidth;
		vt /= m_AtlasHeight;

		glyphData.UVs[0] = { ul, vb };
		glyphData.UVs[1] = { ur, vb };
		glyphData.UVs[2] = { ur, vt };
		glyphData.UVs[3] = { ul, vt };

		double vertL, vertB, vertR, vertT;
		glyph->getQuadPlaneBounds(vertL, vertB, vertR, vertT);

		glyphData.VertexPositions[0] = { vertL, vertB, 0.0f, 1.0f };
		glyphData.VertexPositions[1] = { vertR, vertB, 0.0f, 1.0f };
		glyphData.VertexPositions[2] = { vertR, vertT, 0.0f, 1.0f };
		glyphData.VertexPositions[3] = { vertL, vertT, 0.0f, 1.0f };
			
		glyphData.Advance = static_cast<float>(glyph->getAdvance());

		return glyphData;
	}

	const msdfgen::FontMetrics& Font::GetMetrics() const
	{
		return m_FontGeometry->getMetrics();
	}

	double Font::GetKerning(char c1, char c2) const
	{
		const std::map<std::pair<int, int>, double>& kerningMap = m_FontGeometry->getKerning();
		const msdf_atlas::GlyphGeometry* glyph1 = m_FontGeometry->getGlyph(c1);
		const msdf_atlas::GlyphGeometry* glyph2 = m_FontGeometry->getGlyph(c2);

		if (glyph1 && glyph2) 
		{
			std::pair<int, int> glyphIndicesPair = std::make_pair<int, int>(glyph1->getIndex(), glyph2->getIndex());

			if (kerningMap.contains(glyphIndicesPair)) 
				return kerningMap.at(glyphIndicesPair);
		}

		return 0.0;
	}

	double Font::GetAdvance(char c1, char c2) const
	{
		double advance;
		bool success = m_FontGeometry->getAdvance(advance, c1, c2);

		if (!success)
			return 0.0;

		return advance;
	}

	Shared<Texture2D> Font::LoadFont(const std::string& fontPath)
	{
		msdfgen::FreetypeHandle* freetypeHandle = msdfgen::initializeFreetype();
		TR_ASSERT(freetypeHandle, "Coudln't initialize freetype");

		Shared<Texture2D> texture = nullptr;

		msdfgen::FontHandle* fontHandle = msdfgen::loadFont(freetypeHandle, fontPath.c_str());
		TR_ASSERT(fontHandle, "Couldn't load the font at path: {}", fontPath);

		msdf_atlas::Charset charset = msdf_atlas::Charset::ASCII;
		m_Glyphs = new std::vector<msdf_atlas::GlyphGeometry>();
		m_FontGeometry = new msdf_atlas::FontGeometry(m_Glyphs);

		{
			// the font size is ignored unless there are two or more different font sizes in a single atlas
			int glyphsLoaded = m_FontGeometry->loadCharset(fontHandle, 1.0, charset);

			if (glyphsLoaded < 0)
			{
				TR_CORE_ERROR(TR_LOG_CORE, "Failed to load glyphs from font");

				// NOTE: not uninitializing freetype and not destroying the loaded font
				return texture;
			}

			TR_CORE_TRACE(TR_LOG_CORE, "Loaded geometry of {0} out of {1} glyphs", glyphsLoaded, charset.size());
		}

		if (m_Glyphs->empty())
		{
			TR_CORE_ERROR(TR_LOG_CORE, "No glyphs loaded");
			// NOTE: not uninitializing freetype and not destroying the loaded font
			return texture;
		}

		msdf_atlas::TightAtlasPacker atlasPacker;

		// TODO: make it editable?
		//constexpr msdf_atlas::TightAtlasPacker::DimensionsConstraint dimensionConstraint = msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE;
		//atlasPacker.setDimensionsConstraint(dimensionConstraint);
		atlasPacker.setSpacing(0);
		atlasPacker.setMinimumScale(MSDF_ATLAS_DEFAULT_EM_SIZE);
		//atlasPacker.setScale(40.0);

		constexpr double defaultPixelRange = 2.0;
		atlasPacker.setPixelRange(defaultPixelRange);

		constexpr double defaultMiterLimit = 1.0;
		atlasPacker.setMiterLimit(defaultMiterLimit);

		atlasPacker.pack(m_Glyphs->data(), static_cast<int>(m_Glyphs->size()));

		atlasPacker.getDimensions(m_AtlasWidth, m_AtlasHeight);
		TR_CORE_TRACE(TR_LOG_CORE, "Atlas width: {0}; Atlas height: {1}", m_AtlasWidth, m_AtlasHeight);

		constexpr double maxCornerAngle = 3.0;
		
		for (auto& m_Glyph : *m_Glyphs)
			m_Glyph.edgeColoring(msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

		msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 3>> generator(m_AtlasWidth, m_AtlasHeight);
		msdf_atlas::GeneratorAttributes generatorAttributes;
		generatorAttributes.scanlinePass = true;
		generatorAttributes.config.overlapSupport = true;

		generator.setAttributes(generatorAttributes);
		generator.setThreadCount(std::max(static_cast<int>(std::thread::hardware_concurrency()) / 4, 1));
		generator.generate(m_Glyphs->data(), static_cast<int>(m_Glyphs->size()));

		msdfgen::BitmapConstRef<uint8_t, 3> bitmap = generator.atlasStorage();

		TextureParameters textureParameters;
		textureParameters.Width = bitmap.width;
		textureParameters.Height = bitmap.height;
		textureParameters.Format = TextureFormat::RGB;

		Buffer data = Buffer(bitmap.pixels, bitmap.width * bitmap.height * 3);
		// TODO: cache texture to disk?
		texture = Texture2D::Create(textureParameters, data);

		//m_FontGeometry->getKerning();

		msdfgen::destroyFont(fontHandle);
		
		msdfgen::deinitializeFreetype(freetypeHandle);

		return texture;
	}
}
