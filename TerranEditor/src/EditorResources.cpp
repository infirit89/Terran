#include "EditorResources.h"

#include "Assets/AssetLoaders.h"

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
        FileTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/file_icon.png");
        DirectoryTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/folder_icon.png");
        SettingsTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/more_icon.png");
        PlayTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/play_icon.png");
        StopTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/stop_icon.png");
        ErrorTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/error-icon.png");
        InfoTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/info-icon.png");
        WarningTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/warning-icon.png");
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
