#include "AssetPropertiesPanel.h"

#include "Core/Input.h"

#include "Assets/AssetManager.h"

#include "SelectionManager.h"

#include "UI/UI.h"

#include <imgui.h>

namespace TerranEditor 
{
	using namespace TerranEngine;

	void AssetPropertiesPanel::ImGuiRender()
	{
		if (!m_Open) return;
		ImGui::Begin("Asset Properties", &m_Open);

		UUID assetID = SelectionManager::GetSelectedID(SelectionContext::ContentPanel);
		AssetInfo info = AssetManager::GetAssetInfo(assetID);
		
		if (info.Type == AssetType::PhysicsMaterial2D)
			RenderPhysicsMaterial2DProperties(assetID);

		ImGui::End();
	}

	void AssetPropertiesPanel::OnEvent(Event& event)
	{
		EventDispatcher dispacther(event);
		dispacther.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(AssetPropertiesPanel::OnKeyPressedEvent));
	}

	bool AssetPropertiesPanel::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		UUID assetID = SelectionManager::GetSelectedID(SelectionContext::ContentPanel);

		bool ctrlPressed = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);

		if (e.GetRepeatCount() > 0) return false;

		switch (e.GetKeyCode())
		{
		case Key::S:
			if (ctrlPressed) 
			{
				AssetInfo info = AssetManager::GetAssetInfo(assetID);
				AssetImporter::Save(info, m_CurrentAsset);
				return true;
			}

			break;
		}

		return false;
	}

	void AssetPropertiesPanel::RenderPhysicsMaterial2DProperties(const UUID& assetID)
	{
		Shared<PhysicsMaterial2DAsset> material = AssetManager::GetAsset<PhysicsMaterial2DAsset>(assetID);
		UI::PropertyFloat("Density", material->Density);
		UI::PropertyFloat("Friction", material->Friction);
		UI::PropertyFloat("Restitution", material->Restitution);
		m_CurrentAsset = material;
	}
}
