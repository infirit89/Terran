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

        static TerranEngine::Shared<TerranEngine::Texture> GetPlayTexture();
        static TerranEngine::Shared<TerranEngine::Texture> GetStopTexture();

        static TerranEngine::Shared<TerranEngine::Texture> GetErrorTexture();
        static TerranEngine::Shared<TerranEngine::Texture> GetInfoTexture();
        static TerranEngine::Shared<TerranEngine::Texture> GetWarningTexture();

    private:
        static TerranEngine::Shared<TerranEngine::Texture> s_DirectoryTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_FileTexture;

        static TerranEngine::Shared<TerranEngine::Texture> s_SettingsTexture;
     
        static TerranEngine::Shared<TerranEngine::Texture> s_PlayTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_StopTexture;

        static TerranEngine::Shared<TerranEngine::Texture> s_ErrorTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_InfoTexture;
        static TerranEngine::Shared<TerranEngine::Texture> s_WarningTexture;
	};
}

