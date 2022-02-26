#pragma once

#include "Terran.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	class ECSPanel 
	{
	public:
		ECSPanel() = default;
		~ECSPanel() = default;

		void ImGuiRender();

		void SetContext(const Shared<Scene>& context) { m_Context = context; }

		void SetOpen(bool open) { m_Open = open; }

	private:
		Shared<Scene> m_Context;
		bool m_Open = true;
	};
}