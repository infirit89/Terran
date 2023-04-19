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
		UUID assetID = SelectionManager::GetSelected(SelectionContext::ContentPanel);
		if (!assetID) return;

		AssetInfo assetInfo = AssetManager::GetAssetInfo(assetID);
		if (s_Editors.find(assetInfo.Type) != s_Editors.end())
			s_Editors.at(assetInfo.Type)->OnRender(assetID);
	}

	void AssetEditorManager::RegisterAssetEditors()
	{
		s_Editors[AssetType::PhysicsMaterial2D] = CreateShared<PhysicsMaterialEditor>();
		TR_INFO("Registered all asset editors");
	}
}

