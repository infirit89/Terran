#pragma once

#include "Terran.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class PropertiesPanel 
	{

	public:
		PropertiesPanel() = default;

		void ImGuiRender(Entity& entity);

		inline void SetClosed(bool closed) { m_Closed = closed; }
	private:
		bool m_Closed = true;
	};
}