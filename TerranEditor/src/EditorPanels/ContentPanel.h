#pragma once

#include "Terran.h"
#include <filesystem>

namespace TerranEditor 
{
	using namespace TerranEngine;
	class ContentPanel 
	{
	public:
		ContentPanel() = default;
		ContentPanel(std::filesystem::path resPath);
		~ContentPanel() = default;

		void ImGuiRender();

		void SetOpen(bool open) { m_Open = open; }

	private:
		Shared<Texture> m_DirIcon;
		Shared<Texture> m_FileIcon;

		std::filesystem::path m_AssetPath;
		std::filesystem::path m_CurrentPath;

		bool m_Open = true;
	};
}