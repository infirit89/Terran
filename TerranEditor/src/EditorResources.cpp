#include "EditorResources.h"

namespace TerranEditor 
{
    using namespace TerranEngine;
    
    Shared<Texture2D> EditorResources::FileTexture;
    Shared<Texture2D> EditorResources::DirectoryTexture;

    Shared<Texture2D> EditorResources::SettingsTexture;

    Shared<Texture2D> EditorResources::PlayTexture;
    Shared<Texture2D> EditorResources::StopTexture;

    Shared<Texture2D> EditorResources::ErrorTexture;
    Shared<Texture2D> EditorResources::InfoTexture;
    Shared<Texture2D> EditorResources::WarningTexture;

    void EditorResources::Init()
    {
        FileTexture = CreateShared<Texture2D>("Resources/Textures/file_icon.png");
        DirectoryTexture = CreateShared<Texture2D>("Resources/Textures/folder_icon.png");
        SettingsTexture = CreateShared<Texture2D>("Resources/Textures/more_icon.png");
        PlayTexture = CreateShared<Texture2D>("Resources/Textures/play_icon.png");
        StopTexture = CreateShared<Texture2D>("Resources/Textures/stop_icon.png");
        ErrorTexture = CreateShared<Texture2D>("Resources/Textures/error-icon.png");
        InfoTexture = CreateShared<Texture2D>("Resources/Textures/info-icon.png");
        WarningTexture = CreateShared<Texture2D>("Resources/Textures/warning-icon.png");
    }

    void EditorResources::Shutdown()
    {
        FileTexture = nullptr;
        DirectoryTexture = nullptr;
        SettingsTexture = nullptr;
        PlayTexture = nullptr;
        StopTexture = nullptr;
        ErrorTexture = nullptr;
        InfoTexture = nullptr;
        WarningTexture = nullptr;
    }
}
