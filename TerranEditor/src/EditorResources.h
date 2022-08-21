#pragma once

#include "Core/Base.h"
#include "Graphics/Texture.h"

namespace TerranEditor 
{
	class EditorResources 
	{
    public:
        static void Init();
        static void Shutdown();

        static TerranEngine::Shared<TerranEngine::Texture> GetDirectoryTexture();
        static TerranEngine::Shared<TerranEngine::Texture> GetFileTexture();
        static TerranEngine::Shared<TerranEngine::Texture> GetSettingsTexture();

    private:
        static TerranEngine::Shared<TerranEngine::Texture> s_DirectoryTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_FileTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_SettingsTexture;
	};
}
