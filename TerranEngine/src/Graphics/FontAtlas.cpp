#include "trpch.h"
#include "FontAtlas.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>

namespace TerranEngine 
{
	FontAtlas::FontAtlas(const std::string& fontPath)
	{
		m_Texture = LoadFont(fontPath);
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
				std::vector<msdf_atlas::GlyphGeometry> glyphs;
				msdf_atlas::FontGeometry fontGeometry(&glyphs);
				
				{
					int glyphsLoaded = -1;
					// the font size is ignored unless there are two or more different font sizes in a single atlas
					glyphsLoaded = fontGeometry.loadCharset(fontHandle, 1.0, charset);

					if (glyphsLoaded < 0)
					{
						TR_ERROR("Failed to load glyphs from font");

						// NOTE: not uninitializing freetype and not destroying the loaded font
						return texture;
					}

					TR_TRACE("Loaded geometry of {0} out of {1} glyphs", glyphsLoaded, charset.size());
				}

				if (glyphs.empty())
				{
					TR_ERROR("No glyphs loaded");
					// NOTE: not uninitializing freetype and not destroying the loaded font
					return texture;
				}

				msdf_atlas::TightAtlasPacker atlasPacker;

				// TODO: make it editable?
				constexpr msdf_atlas::TightAtlasPacker::DimensionsConstraint dimensionConstraint = msdf_atlas::TightAtlasPacker::DimensionsConstraint::MULTIPLE_OF_FOUR_SQUARE;
				atlasPacker.setDimensionsConstraint(dimensionConstraint);
				atlasPacker.setPadding(0);
				atlasPacker.setMinimumScale(MSDF_ATLAS_DEFAULT_EM_SIZE);

				const double defaultPixelRange = 2.0;
				atlasPacker.setPixelRange(defaultPixelRange);

				const double defaultMiterLimit = 1.0;
				atlasPacker.setMiterLimit(defaultMiterLimit);

				atlasPacker.pack(glyphs.data(), glyphs.size());

				int atlasWidth = -1, atlasHeight = -1;
				atlasPacker.getDimensions(atlasWidth, atlasHeight);
				TR_TRACE("Atlas width: {0}; Atlas height: {1}", atlasWidth, atlasHeight);

				const double maxCornerAngle = 3.0;
				for (msdf_atlas::GlyphGeometry& glyph : glyphs)
					glyph.edgeColoring(msdfgen::edgeColoringSimple, maxCornerAngle, 0);
				
				msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 3>> generator(atlasWidth, atlasHeight);
				//msdf_atlas::GeneratorAttributes generatorAttributes;
				//generatorAttributes.scanlinePass = true;
				//generatorAttributes.config.overlapSupport = true;

				generator.setThreadCount(std::max((int)std::thread::hardware_concurrency() / 4, 1));
				generator.generate(glyphs.data(), glyphs.size());

				msdfgen::BitmapConstRef<uint8_t, 3> bitmap = generator.atlasStorage();

				TextureParameters params;
				params.TextureType = TextureType::RGB;

				texture = CreateShared<Texture>(bitmap.width, bitmap.height, params);
				texture->SetData(bitmap.pixels);

				msdfgen::destroyFont(fontHandle);
			}

			msdfgen::deinitializeFreetype(freetypeHandle);
		}

		return texture;
	}
}

