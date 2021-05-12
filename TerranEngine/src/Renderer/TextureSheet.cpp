#include "trpch.h"

#include "TextureSheet.h"

namespace TerranEngine
{
	TextureSheet::TextureSheet(char* filePath, int spriteWidth, int spriteHeight, int numSprites)
		: m_Texture(filePath)
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

