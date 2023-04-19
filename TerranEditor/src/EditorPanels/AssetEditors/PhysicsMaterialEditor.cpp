#include "PhysicsMaterialEditor.h"

#include "Assets/AssetManager.h"

#include "UI/UI.h"

#include <imgui.h>

namespace TerranEditor 
{
	using namespace TerranEngine;
	void PhysicsMaterialEditor::OnRender(const TerranEngine::UUID& assetID)
	{
		if (!m_Open) return;
		ImGui::Begin("Physics Material Editor", &m_Open);

		Shared<PhysicsMaterial2DAsset> material = AssetManager::GetAsset<PhysicsMaterial2DAsset>(assetID);
		UI::BeginPropertyGroup("pm2d_properties");
		ImGui::TableNextRow();
		UI::PropertyFloat("Density", material->Density);
		ImGui::TableNextRow();
		UI::PropertyFloat("Friction", material->Friction);
		ImGui::TableNextRow();
		UI::PropertyFloat("Restitution", material->Restitution);
		UI::EndPropertyGroup();

		ImGui::End();
	}
}

