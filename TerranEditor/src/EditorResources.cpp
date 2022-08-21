#include "EditorResources.h"

namespace TerranEditor 
{
    using namespace TerranEngine;
    
    Shared<Texture> EditorResources::s_FileTexture;
    Shared<Texture> EditorResources::s_DirectoryTexture;
    Shared<Texture> EditorResources::s_SettingsTexture;

    void EditorResources::Init()
    {
        s_FileTexture = CreateShared<Texture>("Resources/Textures/file_icon.png");
        s_DirectoryTexture = CreateShared<Texture>("Resources/Textures/folder_icon.png");
        s_SettingsTexture = CreateShared<Texture>("Resources/Textures/more_icon.png");
    }

    void EditorResources::Shutdown()
    {
        s_FileTexture = nullptr;
        s_DirectoryTexture = nullptr;
        s_SettingsTexture = nullptr;
    }

    Shared<Texture> EditorResources::GetDirectoryTexture() { return s_DirectoryTexture; }
    Shared<Texture> EditorResources::GetFileTexture() { return s_FileTexture; }
    TerranEngine::Shared<TerranEngine::Texture> EditorResources::GetSettingsTexture() { return s_SettingsTexture; }
}
