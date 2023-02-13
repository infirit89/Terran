#pragma once

#include "Core/UUID.h"
#include "Events/KeyboardEvent.h"

#include "EditorPanel.h"

namespace TerranEditor 
{
	class AssetPropertiesPanel : public EditorPanel 
	{
	public:
		AssetPropertiesPanel() = default;
		~AssetPropertiesPanel() = default;

		virtual void ImGuiRender() override;
		virtual void OnEvent(TerranEngine::Event& event) override;

	private:
		bool OnKeyPressedEvent(TerranEngine::KeyPressedEvent& kEvent);

		void RenderPhysicsMaterial2DProperties(const TerranEngine::UUID& assetID);

		TerranEngine::Shared<TerranEngine::Asset> m_CurrentAsset;
	};
}