#pragma once

#include "Core/Base.h"
#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include <filesystem>

namespace TerranEditor 
{
	using namespace TerranEngine;
	class ContentPanel : public EditorPanel
	{
	public:
		ContentPanel();
		~ContentPanel() = default;

		virtual void ImGuiRender() override;
	private:
		std::filesystem::path m_CurrentPath;
	};
}
