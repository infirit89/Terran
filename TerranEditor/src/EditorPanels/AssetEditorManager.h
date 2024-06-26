#pragma once

#include "AssetEditors/AssetEditorPanel.h"

#include "Core/UUID.h"
#include "Core/Base.h"
#include "Core/Result.h"

#include "Asset/Asset.h"

#include <unordered_map>

namespace TerranEditor 
{
	class AssetEditorManager
	{
	public:
		AssetEditorManager() = default;
		~AssetEditorManager() = default;

		static void Init();
		static void RenderEditors();
		static TerranEngine::Result OpenAssetEditor(const TerranEngine::UUID& assetHandle);

	private:
		static void RegisterAssetEditors();

	private:
		static std::unordered_map<TerranEngine::AssetType, TerranEngine::Shared<AssetEditorPanel>> s_Editors;
	};
}
