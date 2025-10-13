#pragma once

#include "Graphics/Texture.h"
#include "LibCore/Base.h"

namespace TerranEditor {

class EditorResources {
public:
    static void Init();
    static void Shutdown();

    // general
    static Terran::Core::Shared<TerranEngine::Texture2D> SettingsTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> PlayTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> StopTexture;

    // log textures
    static Terran::Core::Shared<TerranEngine::Texture2D> ErrorTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> InfoTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> WarningTexture;

    // content browser textures
    static Terran::Core::Shared<TerranEngine::Texture2D> DirectoryTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> FileTexture;
    static Terran::Core::Shared<TerranEngine::Texture2D> NavigateNext;
    static Terran::Core::Shared<TerranEngine::Texture2D> NavigateBefore;
    static Terran::Core::Shared<TerranEngine::Texture2D> ChevronRight;
    static Terran::Core::Shared<TerranEngine::Texture2D> Search;

#define TR_DEFAULT_FONT_INDEX 0
#define TR_BOLD_FONT_INDEX 1
};

}
