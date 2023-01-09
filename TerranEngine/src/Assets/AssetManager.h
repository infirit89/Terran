#pragma once

#include "Asset.h"
#include "AssetLoader.h"

#include "Core/Base.h"
#include "Core/FileUtils.h"

#include <unordered_map>
#include <filesystem>

namespace TerranEngine 
{
	class AssetManager 
	{
		using AssetInfoMap = std::unordered_map<UUID, AssetInfo>;
		using AssetChangeCallbackFn = std::function<void(const std::vector<FileSystemChangeEvent>&)>;
	public:
		static void Init();
		static void RegisterAssetLoaders();

		static void Shutdown();

		static const AssetInfo& GetAssetInfo(UUID assetID);
		static const AssetInfo& GetAssetInfo(const std::filesystem::path& assetPath);
		static const UUID& GetAssetID(const std::filesystem::path& assetPath);

		static std::filesystem::path GetFileSystemPath(const std::filesystem::path& path);

		static AssetInfoMap& GetAssetInfoMap() { return s_AssetsInfos; }

		static UUID ImportAsset(const std::filesystem::path& assetPath);
		static void ReloadAsset(UUID assetID);

		inline static void SetAssetChangedCallback(AssetChangeCallbackFn callback) 
		{
			s_ChangeCallback = callback;
		}

		template<typename T>
		inline static Shared<T> GetAsset(const UUID& assetID) 
		{
			if (s_LoadedAssets.find(assetID) != s_LoadedAssets.end())
				return DynamicCast<T>(s_LoadedAssets.at(assetID));
			else 
			{
				AssetInfo& info = GetAssetInfo_Internal(assetID);

				// NOTE: poc code
				Shared<Asset> asset = nullptr;
				s_Loaders[info.Type]->Load(info, asset);

				if (!asset)
				{
					TR_ERROR("Failed to load asset with path: {0}", info.Path);
					return nullptr;
				}

				asset->m_Handle = assetID;
				s_LoadedAssets[assetID] = asset;
				return DynamicCast<T>(s_LoadedAssets[assetID]);
			}
		}

	private:
		static void WriteAssetInfosToFile();
		static void ReadAssetInfos();
		static void OnFileSystemChanged(const std::vector<FileSystemChangeEvent>& fileSystemEvents);
		static void OnAssetRemoved(UUID assetID);
		static void OnAssetRenamed(UUID assetID, const std::filesystem::path& newFileName);
		static AssetInfo& GetAssetInfo_Internal(const UUID& assetID);
		static std::filesystem::path GetRelativePath(const std::filesystem::path& path);

	private:
		static std::unordered_map<UUID, Shared<Asset>> s_LoadedAssets;
		static AssetInfoMap s_AssetsInfos;
		static std::unordered_map<AssetType, Shared<AssetLoader>> s_Loaders;
		static AssetChangeCallbackFn s_ChangeCallback;
	};
}
