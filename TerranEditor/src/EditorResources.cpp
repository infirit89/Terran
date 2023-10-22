#include "EditorResources.h"

#include "Assets/AssetLoaders.h"

#include "IconsFontAwesome6.h"
#include "IconsMaterialDesign.h"

#include "UI/FontManager.h"
#include <imgui.h>

namespace TerranEditor 
{
    using namespace TerranEngine;
    
    // content browser textures;
    Shared<Texture2D> EditorResources::FileTexture;
    Shared<Texture2D> EditorResources::DirectoryTexture;
    Shared<Texture2D> EditorResources::NavigateNext;
    Shared<Texture2D> EditorResources::NavigateBefore;

    // general
    Shared<Texture2D> EditorResources::SettingsTexture;
    Shared<Texture2D> EditorResources::PlayTexture;
    Shared<Texture2D> EditorResources::StopTexture;

    // log textures
    Shared<Texture2D> EditorResources::ErrorTexture;
    Shared<Texture2D> EditorResources::InfoTexture;
    Shared<Texture2D> EditorResources::WarningTexture;

    void EditorResources::Init()
    {
        // general
        SettingsTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/more_icon.png");
        PlayTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/play_icon.png");
        StopTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/stop_icon.png");

        // log textures
        ErrorTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/error-icon.png");
        InfoTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/info-icon.png");
        WarningTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/warning-icon.png");

        // content browser textures
        DirectoryTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/folder_icon.png");
        FileTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/file_icon.png");
        NavigateNext = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/navigate_next.png");
        NavigateBefore = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/navigate_before.png");


        constexpr float baseFontSize = 16.0f;
        // default font
        UI::FontConfig defaultFontConfig;
        defaultFontConfig.Name = "Roboto-Regular";
        defaultFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Regular.ttf";
        UI::FontManager::AddFont(defaultFontConfig, true);

        // bold font
        UI::FontConfig boldFontConfig;
        boldFontConfig.Name = "Roboto-Bold";
        boldFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Bold.ttf";
        UI::FontManager::AddFont(boldFontConfig);

        // icon font
        UI::FontConfig fontAwesomeConfigLarge;
        constexpr float iconFontSize = baseFontSize;
        fontAwesomeConfigLarge.Name = "IconFont";
        fontAwesomeConfigLarge.Path = "Resources/Fonts/FontAwesome/fa-solid-900.ttf";
        fontAwesomeConfigLarge.Size = 16.0f;
        fontAwesomeConfigLarge.MergeMode = true;
        fontAwesomeConfigLarge.PixelSnapH = true;
        fontAwesomeConfigLarge.GlyphMixAdvanceX = iconFontSize;
        const ImWchar faIconsRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        fontAwesomeConfigLarge.GlyphRange = faIconsRange;
        UI::FontManager::AddFont(fontAwesomeConfigLarge);

        UI::FontManager::BuildFontAtlas();

        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "Resources/TerranEditorSettings.ini";
    }

    void EditorResources::Shutdown()
    {
        // content browser textures
        FileTexture = nullptr;
        DirectoryTexture = nullptr;
        NavigateNext = nullptr;
        NavigateBefore = nullptr;

        // general
        SettingsTexture = nullptr;
        PlayTexture = nullptr;
        StopTexture = nullptr;

        // log textures
        ErrorTexture = nullptr;
        InfoTexture = nullptr;
        WarningTexture = nullptr;
    }
}
