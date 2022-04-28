#pragma once

#include "Texture.h"
#include "Core/Base.h"

#include <string>

namespace TerranEngine 
{
	class FontAtlas 
	{
	public:
		FontAtlas() = default;
		FontAtlas(const std::string& fontPath);
		~FontAtlas() = default;

		inline Shared<Texture> GetTexture() const { return m_Texture; }

	private:
		Shared<Texture> LoadFont(const std::string& fontPath);

	private:
		Shared<Texture> m_Texture;
	};
}