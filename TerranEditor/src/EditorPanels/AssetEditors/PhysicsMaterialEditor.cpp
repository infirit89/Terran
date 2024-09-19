#include "PhysicsMaterialEditor.h"

#include "Asset/AssetManager.h"
#include "Physics/PhysicsMaterial.h"

#include "UI/UI.h"

#include <imgui.h>

namespace TerranEditor 
{
	using namespace TerranEngine;
	void PhysicsMaterialEditor::OnRender()
	{
		if (!m_Open) return;
		ImGui::Begin("Physics Material Editor", &m_Open);

		bool changed = false;
		Shared<PhysicsMaterial2D> material = AssetManager::GetAssetByHandle<PhysicsMaterial2D>(m_ID);
		if (material) 
		{
			UI::PropertyGroup("pm2d_properties", [material, &changed]()
			{
				ImGui::TableNextRow();
				changed |= UI::PropertyFloat("Density", material->Density);
				ImGui::TableNextRow();
				changed |= UI::PropertyFloat("Friction", material->Friction);
				ImGui::TableNextRow();
				changed |= UI::PropertyFloat("Restitution", material->Restitution);
			});
		}

		if (changed)
			AssetImporter::Save(AssetManager::GetAssetInfoByHandle(m_ID), material);

		ImGui::End();
	}
}
