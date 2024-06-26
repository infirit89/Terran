#include "AssetEditorManager.h"

#include "Asset/AssetManager.h"

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

	Result AssetEditorManager::OpenAssetEditor(const UUID& assetHandle)
	{
		if (!assetHandle.IsValid())
			return Result::INVALID_HANDLE;

		AssetInfo assetInfo = AssetManager::GetAssetInfo(assetHandle);

		if (s_Editors.find(assetInfo.Type) == s_Editors.end())
			return Result::NOT_FOUND;

		s_Editors.at(assetInfo.Type)->SetOpen(true, assetHandle);
		return Result::OK;
	}

	void AssetEditorManager::RegisterAssetEditors()
	{
		s_Editors[AssetType::PhysicsMaterial2D] = CreateShared<PhysicsMaterialEditor>();
		TR_CORE_INFO(TR_LOG_CORE, "Registered all asset editors");
	}
}

