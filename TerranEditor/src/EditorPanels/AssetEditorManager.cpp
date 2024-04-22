#include "AssetEditorManager.h"

#include "Assets/AssetManager.h"

#include "AssetEditors/PhysicsMaterialEditor.h"
#include "SelectionManager.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	std::unordered_map<TerranEngine::AssetType, TerranEngine::Shared<AssetEditorPanel>> AssetEditorManager::s_Editors;

	void AssetEditorManager::Init()
	{
		RegisterAssetEditors();
	}

	void AssetEditorManager::RenderEditors()
	{
		// TODO: add multiselect functionality
		/*const std::vector<UUID>& selected = SelectionManager::GetSelected(SelectionContext::ContentPanel);
		if (!selected.empty()) 
		{
			UUID assetHandle = selected[0];
			
		}*/

		for (const auto& [assetType, assetEditorPanel] : s_Editors) 
			assetEditorPanel->OnRender();
	}

	void AssetEditorManager::OpenAssetEditor(const UUID& assetHandle)
	{
		if (!assetHandle.IsValid())
			return;

		AssetInfo assetInfo = AssetManager::GetAssetInfo(assetHandle);
		if (s_Editors.find(assetInfo.Type) != s_Editors.end())
			s_Editors.at(assetInfo.Type)->SetOpen(true, assetHandle);
	}

	void AssetEditorManager::RegisterAssetEditors()
	{
		s_Editors[AssetType::PhysicsMaterial2D] = CreateShared<PhysicsMaterialEditor>();
		TR_CORE_INFO(TR_LOG_CORE, "Registered all asset editors");
	}
}

