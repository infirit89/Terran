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
		TextureSheet(char* filePath, uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites);
		TextureSheet(void* data, uint32_t textureWidth, uint32_t textureHeight, uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites);

		glm::vec2* GetUVS(int index) 
		{
			return m_UVS[index].data();
		}

		glm::vec2* operator[](int index) 
		{
			return m_UVS[index].data();
		}

		const Texture& GetTexture() const { return m_Texture; }

	private:
		void Init(uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites);

	private:
		Texture m_Texture;

		std::vector<std::array<glm::vec2, 4>> m_UVS;
	};
}