#pragma once

#include "Core/Base.h"
#include "Graphics/Texture.h"

namespace TerranEditor 
{
	class Resources 
	{
    public:
        void Init();
        void Shutdown();

        TerranEngine::Shared<TerranEngine::Texture> GetDirectoryTexture();
        TerranEngine::Shared<TerranEngine::Texture> GetFileTexture();
	};
}
