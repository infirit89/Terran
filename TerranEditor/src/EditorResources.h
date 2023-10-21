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

        // general
        static TerranEngine::Shared<TerranEngine::Texture2D> SettingsTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> PlayTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> StopTexture;

        // log textures
        static TerranEngine::Shared<TerranEngine::Texture2D> ErrorTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> InfoTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> WarningTexture;

        // content browser textures
        static TerranEngine::Shared<TerranEngine::Texture2D> DirectoryTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> FileTexture;
        static TerranEngine::Shared<TerranEngine::Texture2D> NavigateNext;
        static TerranEngine::Shared<TerranEngine::Texture2D> NavigateBefore;

#define TR_DEFAULT_FONT_INDEX 0
#define TR_BOLD_FONT_INDEX 1
	};
}

