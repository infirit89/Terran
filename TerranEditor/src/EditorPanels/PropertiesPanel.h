#pragma once

#include "Core/Base.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptArray.h"
#include "Scripting/GCManager.h"

#include "EditorPanel.h"

#include "SelectionManager.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class PropertiesPanel : public EditorPanel
	{
	public:
		PropertiesPanel() = default;
		~PropertiesPanel() = default;

		virtual void OnRender() override;
		virtual const char* GetName() override { return "Properties"; }

	private:
		void DrawComponents();
	};
}
