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

		void SetCurrentPath(const std::filesystem::path& currentPath) { m_CurrentPath = currentPath; }
	private:
		Shared<Texture> m_DirIcon;
		Shared<Texture> m_FileIcon;

		std::filesystem::path m_CurrentPath;
	};
}