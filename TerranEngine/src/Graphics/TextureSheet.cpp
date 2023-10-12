#include "trpch.h"
#include "TextureSheet.h"

#if todo
#pragma warning (push)
#pragma warning (disable : 4244)

namespace TerranEngine
{
	TextureSheet::TextureSheet(char* filePath, uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites)
		: m_Texture(filePath, {})
	{
		Init(spriteWidth, spriteHeight, numSprites);
	}

	TextureSheet::TextureSheet(void* data, uint32_t textureWidth, uint32_t textureHeight, uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites)
		: m_Texture(textureWidth, textureHeight, {})
	{
		m_Texture.SetData(data);

		Init(spriteWidth, spriteHeight, numSprites);
	}

	void TextureSheet::Init(uint32_t spriteWidth, uint32_t spriteHeight, uint16_t numSprites)
	{
		m_UVS.reserve(numSprites);

		float currentX = 0;
		float currentY = m_Texture.GetHeight();

		for (size_t i = 0; i < numSprites; i++)
		{
			float bottomY = (currentY - spriteHeight) / m_Texture.GetHeight();
			float leftX = currentX / m_Texture.GetWidth();

			float topY = currentY / m_Texture.GetHeight();
			float rightX = (currentX + spriteWidth) / m_Texture.GetWidth();

			std::array<glm::vec2, 4> uvs;

			uvs[0] = { leftX,  bottomY };
			uvs[1] = { rightX, bottomY };
			uvs[2] = { rightX, topY };
			uvs[3] = { leftX,  topY };

			m_UVS.emplace_back(uvs);

			currentX += spriteWidth;

			if (currentX >= m_Texture.GetWidth())
			{
				currentX = 0;
				currentY -= spriteHeight;
			}
		}
	}
}
#pragma warning (pop)
#endif
