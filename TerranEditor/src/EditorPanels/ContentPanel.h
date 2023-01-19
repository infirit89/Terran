#pragma once

#include "Core/Base.h"
#include "Graphics/Texture.h"

#include "EditorPanel.h"

#include <filesystem>

namespace TerranEditor 
{
	using namespace TerranEngine;

	struct DirectoryInfo 
	{
		std::filesystem::path Path;
		std::vector<Shared<DirectoryInfo>> Subdirectories;
	};

	class ContentPanel : public EditorPanel
	{
	public:
		ContentPanel();
		~ContentPanel() = default;

		virtual void ImGuiRender() override;

		virtual void OnProjectChanged(const std::filesystem::path& projectPath) override;
	private:
		std::filesystem::path m_CurrentPath;
		std::unordered_map<UUID, Shared<DirectoryInfo>> m_DirectoryInfoMap;
	};
}
