#pragma once

#include "Core/Base.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

#include "Scripting/ScriptField.h"
#include "Scripting/ScriptArray.h"
#include "Scripting/GCManager.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class PropertiesPanel 
	{

	public:
		PropertiesPanel() = default;
		~PropertiesPanel() = default;

		void ImGuiRender(Entity entity);
		inline void SetOpen(bool open) { m_Open = open; }

	private:
		void DrawScriptField(const Shared<Scene>& scene, ScriptField* field, const GCHandle& handle);
		bool DrawScriptArrayField(const Shared<Scene>& scene, const std::string& fieldName, ScriptArray& array);

		bool m_Open = true;
	};
}