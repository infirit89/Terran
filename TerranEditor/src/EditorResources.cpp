#include "EditorResources.h"

#include "Asset/AssetLoaders.h"

#include "IconsFontAwesome6.h"
#include "IconsMaterialDesign.h"

#include "UI/FontManager.h"
#include <imgui.h>

namespace TerranEditor {

using namespace TerranEngine;

// content browser textures;
Terran::Core::Shared<Texture2D> EditorResources::FileTexture;
Terran::Core::Shared<Texture2D> EditorResources::DirectoryTexture;
Terran::Core::Shared<Texture2D> EditorResources::NavigateNext;
Terran::Core::Shared<Texture2D> EditorResources::NavigateBefore;
Terran::Core::Shared<Texture2D> EditorResources::ChevronRight;
Terran::Core::Shared<Texture2D> EditorResources::Search;

// general
Terran::Core::Shared<Texture2D> EditorResources::SettingsTexture;
Terran::Core::Shared<Texture2D> EditorResources::PlayTexture;
Terran::Core::Shared<Texture2D> EditorResources::StopTexture;

// log textures
Terran::Core::Shared<Texture2D> EditorResources::ErrorTexture;
Terran::Core::Shared<Texture2D> EditorResources::InfoTexture;
Terran::Core::Shared<Texture2D> EditorResources::WarningTexture;

void EditorResources::Init()
{
    // general
    SettingsTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/more_icon.png");
    PlayTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/play_icon.png");
    StopTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/stop_icon.png");

    // log textures
    ErrorTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/error_icon.png");
    InfoTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/info_icon.png");
    WarningTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/warning_icon.png");

    // content browser textures
    DirectoryTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/folder_icon.png");
    FileTexture = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/file_icon.png");
    NavigateNext = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/navigate_next.png");
    NavigateBefore = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/navigate_before.png");
    ChevronRight = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/chevron_right.png");
    Search = TextureAssetLoader::CreateTextureFromFile("Resources/Textures/search_icon.png");

    // medium font
    UI::FontConfig mediumFontConfig;
    mediumFontConfig.Name = "Roboto-Regular_Medium";
    mediumFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Regular.ttf";
    mediumFontConfig.Size = 18.0f;
    UI::FontManager::AddFont(mediumFontConfig);

    // bold font
    UI::FontConfig boldFontConfig;
    boldFontConfig.Name = "Roboto-Bold";
    boldFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Bold.ttf";
    UI::FontManager::AddFont(boldFontConfig);

    // default font
    UI::FontConfig defaultFontConfig;
    defaultFontConfig.Name = "Roboto-Regular";
    defaultFontConfig.Path = "Resources/Fonts/Roboto/Roboto-Regular.ttf";
    UI::FontManager::AddFont(defaultFontConfig, true);

    // icon font
    UI::FontConfig fontAwesomeConfigLarge;
    constexpr float iconFontSize = 16.0f;
    fontAwesomeConfigLarge.Name = "IconFont";
    fontAwesomeConfigLarge.Path = "Resources/Fonts/FontAwesome/fa-solid-900.ttf";
    fontAwesomeConfigLarge.Size = iconFontSize;
    fontAwesomeConfigLarge.MergeMode = true;
    fontAwesomeConfigLarge.PixelSnapH = true;
    fontAwesomeConfigLarge.GlyphMixAdvanceX = iconFontSize;
    ImWchar const faIconsRange[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
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
    ChevronRight = nullptr;
    Search = nullptr;

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
