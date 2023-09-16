#pragma once

#include "AssetEditors/AssetEditorPanel.h"

#include "Core/UUID.h"
#include "Core/Base.h"

#include "Assets/Asset.h"

#include <unordered_map>

namespace TerranEditor 
{
	class AssetEditorManager
	{
	public:
		AssetEditorManager() = default;
		~AssetEditorManager() = default;

		//void ImGuiRender() override;
		//virtual void OnEvent(TerranEngine::Event& event) override;

		static void Init();
		static void RenderEditors();

	private:
		static void RegisterAssetEditors();

	private:
		static std::unordered_map<TerranEngine::AssetType, TerranEngine::Shared<AssetEditorPanel>> s_Editors;
	};
}