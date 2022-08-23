#include "EditorResources.h"

namespace TerranEditor 
{
    using namespace TerranEngine;
    
    Shared<Texture> EditorResources::s_FileTexture;
    Shared<Texture> EditorResources::s_DirectoryTexture;

    Shared<Texture> EditorResources::s_SettingsTexture;

    Shared<Texture> EditorResources::s_PlayTexture;
    Shared<Texture> EditorResources::s_StopTexture;

    Shared<Texture> EditorResources::s_ErrorTexture;
    Shared<Texture> EditorResources::s_InfoTexture;
    Shared<Texture> EditorResources::s_WarningTexture;

    void EditorResources::Init()
    {
        s_FileTexture = CreateShared<Texture>("Resources/Textures/file_icon.png");
        s_DirectoryTexture = CreateShared<Texture>("Resources/Textures/folder_icon.png");
        s_SettingsTexture = CreateShared<Texture>("Resources/Textures/more_icon.png");
        s_PlayTexture = CreateShared<Texture>("Resources/Textures/play_icon.png");
        s_StopTexture = CreateShared<Texture>("Resources/Textures/stop_icon.png");
        s_ErrorTexture = CreateShared<Texture>("Resources/Textures/error-icon.png");
        s_InfoTexture = CreateShared<Texture>("Resources/Textures/info-icon.png");
        s_WarningTexture = CreateShared<Texture>("Resources/Textures/warning-icon.png");
    }

    void EditorResources::Shutdown()
    {
        s_FileTexture = nullptr;
        s_DirectoryTexture = nullptr;
        s_SettingsTexture = nullptr;
        s_PlayTexture = nullptr;
        s_StopTexture = nullptr;
        s_ErrorTexture = nullptr;
        s_InfoTexture = nullptr;
        s_WarningTexture = nullptr;
    }

    Shared<Texture> EditorResources::GetDirectoryTexture() { return s_DirectoryTexture; }
    Shared<Texture> EditorResources::GetFileTexture() { return s_FileTexture; }
    Shared<Texture> EditorResources::GetSettingsTexture() { return s_SettingsTexture; }
    Shared<Texture> EditorResources::GetPlayTexture() { return s_PlayTexture; }
    Shared<Texture> EditorResources::GetStopTexture() { return s_StopTexture; }
    Shared<Texture> EditorResources::GetErrorTexture() { return s_ErrorTexture; }
    Shared<Texture> EditorResources::GetInfoTexture() { return s_InfoTexture; }
    Shared<Texture> EditorResources::GetWarningTexture() { return s_WarningTexture; }
}

