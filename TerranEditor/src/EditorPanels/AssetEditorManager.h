#pragma once

#include "AssetEditors/AssetEditorPanel.h"

#include "Core/Result.h"
#include "LibCore/Base.h"
#include "LibCore/UUID.h"

#include "Asset/Asset.h"

#include <unordered_map>

namespace TerranEditor {

class AssetEditorManager {
public:
    AssetEditorManager() = default;
    ~AssetEditorManager() = default;

    static void Init();
    static void RenderEditors();
    static TerranEngine::Result OpenAssetEditor(Terran::Core::UUID const& assetHandle);

private:
    static void RegisterAssetEditors();

private:
    static std::unordered_map<TerranEngine::AssetType, Terran::Core::Shared<AssetEditorPanel>> s_Editors;
};

}
