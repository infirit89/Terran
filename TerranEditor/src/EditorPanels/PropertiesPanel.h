#pragma once

#include "Core/Base.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "EditorPanel.h"

#include "SelectionManager.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class PropertiesPanel : public EditorPanel
	{
	public:
		PropertiesPanel();
		~PropertiesPanel() = default;

		virtual void OnRender() override;
		virtual std::string_view GetName() override { return "Properties"; }

	private:
		void DrawComponents();
	};
}
