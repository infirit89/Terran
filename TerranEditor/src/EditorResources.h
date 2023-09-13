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

        static TerranEngine::Shared<TerranEngine::Texture2D> DirectoryTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> FileTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> SettingsTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> PlayTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> StopTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> ErrorTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> InfoTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> WarningTexture;
	};
}

