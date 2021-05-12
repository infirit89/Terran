#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "Texture.h"

namespace TerranEngine 
{
	class TextureSheet 
	{
	public:
		TextureSheet(char* filePath, int spriteWidth, int spriteHeight, int numSprites);

		glm::vec2* GetUVS(int index) 
		{
			return m_UVS[index].data();
		}

		const Texture& GetTexture() const { return m_Texture; }

	private:
		Texture m_Texture;

		std::vector<std::array<glm::vec2, 4>> m_UVS;
	};
}