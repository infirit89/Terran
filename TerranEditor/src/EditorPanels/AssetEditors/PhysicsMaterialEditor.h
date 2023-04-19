#pragma once

#include "AssetEditorPanel.h"

namespace TerranEditor 
{
	class PhysicsMaterialEditor : public AssetEditorPanel
	{
	public:
		PhysicsMaterialEditor() = default;
		virtual ~PhysicsMaterialEditor() override = default;

		virtual void OnRender(const TerranEngine::UUID& assetID) override;
	};
}