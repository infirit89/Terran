#pragma once

#include "Scene/Entity.h"

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
		bool m_Open = true;
	};
}