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
        UI::FontConfig defaultFontConfig;
        defaultFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Regular.ttf";
        UI::FontManager::AddFont(defaultFontConfig, true);

        UI::FontConfig boldFontConfig;
        boldFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Bold.ttf";
        UI::FontManager::AddFont(boldFontConfig);

        UI::FontManager::BuildFontAtlas();

        /*ImFontConfig config;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", baseFontSize, &config);
        io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Bold.ttf", baseFontSize, &config);

        const float iconFontSize = baseFontSize * 2.0f / 3.0f;
        static const ImWchar faIconsRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        ImFontConfig iconsConfig;
        iconsConfig.MergeMode = true;
        iconsConfig.PixelSnapH = true;
        iconsConfig.GlyphMinAdvanceX = iconFontSize;
        io.Fonts->AddFontFromFileTTF("Resources/Fonts/FontAwesome/FontAwesome6Regular.otf", iconFontSize, &iconsConfig, faIconsRange);

        io.Fonts->Build();*/
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
