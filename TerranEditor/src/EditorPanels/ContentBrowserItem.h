#pragma once

#include "Core/Base.h"
#include "Core/UUID.h"

#include "Graphics/Texture.h"

#include "Assets/AssetManager.h"

#include <string>
#include <filesystem>

namespace TerranEditor 
{
	enum class ItemAction
	{
		None = 0,
		NavigateTo,
		Activate,
		Select,
		StartRename,
		Renamed,
		MoveTo
	};

	enum class ItemType
	{
		Directory = 0,
		File
	};

	class ContentBrowserItem
	{
	public:
		ContentBrowserItem(const std::string& name, const TerranEngine::UUID& handle, TerranEngine::Shared<TerranEngine::Texture> icon, ItemType type);
		virtual ~ContentBrowserItem() = default;

		void BeginRender();
		void EndRender();
		ItemAction OnRender();
		virtual void Move(const std::filesystem::path& newPath) = 0;

		TerranEngine::UUID GetHandle() { return m_Handle; }
		const std::string& GetName() { return m_Name; }
		ItemType GetType() { return m_Type; }

		void StartRename();
		void StopRename();

	private:
		virtual ItemAction OnActivate() = 0;
		virtual ItemAction OnRename(const std::string& newName) = 0;

	private:
		std::string m_Name;
		TerranEngine::Shared<TerranEngine::Texture> m_Icon;
		TerranEngine::UUID m_Handle;
		ItemType m_Type;
		bool m_IsRenaming = false;

		friend class ContentPanel;
	};

	struct DirectoryInfo;
	class ContentBrowserDirectory : public ContentBrowserItem
	{
	public:
		ContentBrowserDirectory(const TerranEngine::Shared<DirectoryInfo>& directoryInfo);
		virtual ~ContentBrowserDirectory() override = default;

		virtual void Move(const std::filesystem::path& newPath) override;
		const TerranEngine::Shared<DirectoryInfo>& GetDirectoryInfo() { return m_DirectoryInfo; }

	private:
		virtual ItemAction OnActivate() override;
		virtual ItemAction OnRename(const std::string& newName) override;

	private:
		TerranEngine::Shared<DirectoryInfo> m_DirectoryInfo;
	};

	class ContentBrowserAsset : public ContentBrowserItem
	{
	public:
		ContentBrowserAsset(const TerranEngine::AssetInfo& assetInfo, const TerranEngine::Shared<TerranEngine::Texture>& icon);
		virtual ~ContentBrowserAsset() override = default;

		virtual void Move(const std::filesystem::path& newPath) override;
		const TerranEngine::AssetInfo& GetAssetInfo() { return m_AssetInfo; }

	private:
		virtual ItemAction OnActivate() override;
		virtual ItemAction OnRename(const std::string& newName) override;

	private:
		TerranEngine::AssetInfo m_AssetInfo;
	};
}
