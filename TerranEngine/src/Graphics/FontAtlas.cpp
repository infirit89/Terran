#include "trpch.h"
#include "FontAtlas.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>

namespace TerranEngine 
{
	FontAtlas::FontAtlas()
		: m_Glyphs(nullptr), m_FontGeometry(nullptr), m_AtlasWidth(-1), m_AtlasHeight(-1)
	{
	}

	FontAtlas::FontAtlas(const std::string& fontPath)
		: m_Glyphs(nullptr), m_FontGeometry(nullptr), m_AtlasWidth(-1), m_AtlasHeight(-1)
	{
		m_Texture = LoadFont(fontPath);
	}

	FontAtlas::~FontAtlas()
	{
		if (m_Glyphs)
			delete m_Glyphs;

		if (m_FontGeometry)
			delete m_FontGeometry;
	}

	GlyphData FontAtlas::GetGlyphData(char c)
	{
		const msdf_atlas::GlyphGeometry* glyph = m_FontGeometry->getGlyph(c);

		if (glyph) 
		{
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
			
			glyphData.Advance = (float)glyph->getAdvance();

			return glyphData;
		}

		return {};
	}

	double FontAtlas::GetKerning(char c1, char c2)
	{
		std::map<std::pair<int, int>, double> kerningMap = m_FontGeometry->getKerning();
		const msdf_atlas::GlyphGeometry* glyph1,* glyph2;
		glyph1 = m_FontGeometry->getGlyph(c1);
		glyph2 = m_FontGeometry->getGlyph(c2);

		if (glyph1 && glyph2) 
		{
			std::pair<int, int> glyphIndicesPair = std::make_pair<int, int>(glyph1->getIndex(), glyph2->getIndex());

			if (kerningMap.find(glyphIndicesPair) != kerningMap.end()) 
			{
				TR_TRACE("Char 1: {0}, Char 2: {1}, Kerning: {2}", c1, c2, kerningMap.at(glyphIndicesPair));
				return kerningMap.at(glyphIndicesPair);
			}
		}

		return 0.0;
	}

	Shared<Texture> FontAtlas::LoadFont(const std::string& fontPath)
	{
		msdfgen::FreetypeHandle* freetypeHandle = msdfgen::initializeFreetype();

		Shared<Texture> texture = nullptr;

		if (freetypeHandle) 
		{
			msdfgen::FontHandle* fontHandle = msdfgen::loadFont(freetypeHandle, fontPath.c_str());
			if (fontHandle)
			{
				msdf_atlas::Charset charset = msdf_atlas::Charset::ASCII;
				m_Glyphs = new std::vector<msdf_atlas::GlyphGeometry>();
				m_FontGeometry = new msdf_atlas::FontGeometry(m_Glyphs);

				{
					int glyphsLoaded = -1;
					// the font size is ignored unless there are two or more different font sizes in a single atlas
					glyphsLoaded = m_FontGeometry->loadCharset(fontHandle, 1.0, charset);

					if (glyphsLoaded < 0)
					{
						TR_ERROR("Failed to load glyphs from font");

						// NOTE: not uninitializing freetype and not destroying the loaded font
						return texture;
					}

					TR_TRACE("Loaded geometry of {0} out of {1} glyphs", glyphsLoaded, charset.size());
				}

				if (m_Glyphs->empty())
				{
					TR_ERROR("No glyphs loaded");
					// NOTE: not uninitializing freetype and not destroying the loaded font
					return texture;
				}

				msdf_atlas::TightAtlasPacker atlasPacker;

				// TODO: make it editable?
				constexpr msdf_atlas::TightAtlasPacker::DimensionsConstraint dimensionConstraint = msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE;
				atlasPacker.setDimensionsConstraint(dimensionConstraint);
				atlasPacker.setPadding(1);
				//atlasPacker.setMinimumScale(MSDF_ATLAS_DEFAULT_EM_SIZE);
				atlasPacker.setMinimumScale(MSDF_ATLAS_DEFAULT_EM_SIZE * 2);

				const double defaultPixelRange = 2.0;
				atlasPacker.setPixelRange(defaultPixelRange);

				const double defaultMiterLimit = 1.0;
				atlasPacker.setMiterLimit(defaultMiterLimit);

				atlasPacker.pack(m_Glyphs->data(), m_Glyphs->size());

				atlasPacker.getDimensions(m_AtlasWidth, m_AtlasHeight);
				TR_TRACE("Atlas width: {0}; Atlas height: {1}", m_AtlasWidth, m_AtlasHeight);

				const double maxCornerAngle = 3.0;
				
				for (size_t i = 0; i < m_Glyphs->size(); i++)
					(*m_Glyphs)[i].edgeColoring(msdfgen::edgeColoringSimple, maxCornerAngle, 0);

				msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 3>> generator(m_AtlasWidth, m_AtlasHeight);
				//msdf_atlas::GeneratorAttributes generatorAttributes;
				//generatorAttributes.scanlinePass = true;
				//generatorAttributes.config.overlapSupport = true;

				generator.setThreadCount(std::max((int)std::thread::hardware_concurrency() / 4, 1));
				generator.generate(m_Glyphs->data(), m_Glyphs->size());

				msdfgen::BitmapConstRef<uint8_t, 3> bitmap = generator.atlasStorage();

				TextureParameters params;
				params.TextureType = TextureType::RGB;

				texture = CreateShared<Texture>(bitmap.width, bitmap.height, params);
				texture->SetData(bitmap.pixels);

				m_FontGeometry->getKerning();

				msdfgen::destroyFont(fontHandle);
			}

			msdfgen::deinitializeFreetype(freetypeHandle);
		}

		return texture;
	}
}
