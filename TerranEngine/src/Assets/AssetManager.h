#pragma once

#include "Asset.h"
#include "AssetImporter.h"

#include "Core/Base.h"
#include "Core/FileUtils.h"
#include "Core/Log.h"

#include <unordered_map>
#include <map>
#include <filesystem>

namespace TerranEngine 
{
	class AssetManager 
	{
		using AssetInfoMap = std::map<UUID, AssetInfo>;
		using AssetChangeCallbackFn = std::function<void(const std::vector<FileSystemChangeEvent>&)>;
	public:
		static void Init();
		
		static void Shutdown();

		static const AssetInfo& GetAssetInfo(UUID assetID);
		static const AssetInfo& GetAssetInfo(const std::filesystem::path& assetPath);
		static UUID GetAssetID(const std::filesystem::path& assetPath);

		static std::filesystem::path GetFileSystemPath(const std::filesystem::path& path);

		static AssetInfoMap& GetAssetInfoMap() { return s_AssetsInfos; }

		static UUID ImportAsset(const std::filesystem::path& assetPath);
		static void ReloadAsset(UUID assetID);

		inline static void SetAssetChangedCallback(AssetChangeCallbackFn callback) { s_ChangeCallback = callback; }

		template<typename T>
		inline static Shared<T> GetAsset(const UUID& assetID) 
		{
			if (s_LoadedAssets.find(assetID) != s_LoadedAssets.end())
				return DynamicCast<T>(s_LoadedAssets.at(assetID));
			else 
			{
				AssetInfo& info = GetAssetInfo_Internal(assetID);

				if (!info)
					return nullptr;

				// NOTE: poc code
				Shared<Asset> asset = nullptr;
				AssetImporter::Load(info, asset);
				
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

		template<typename T>
		static Shared<T> CreateNewAsset(const std::filesystem::path& filePath) 
		{
			AssetInfo info;
			info.Handle = UUID();
			info.Path = filePath;
			info.Type = T::GetStaticType();

			int currentFileNumber = 2;
			while (FileExists(info.Path)) 
			{
				info.Path = filePath.parent_path() / filePath.stem();
					
				info.Path = info.Path.string() +
								" (" + std::to_string(currentFileNumber) + ")" +
								filePath.extension().string();


				currentFileNumber++;
			}

			s_AssetsInfos[info.Handle] = info;

			// TODO: add parameter options
			Shared<T> asset = CreateShared<T>();

			s_LoadedAssets[info.Handle] = asset;
			AssetImporter::Save(info, asset);
			
			return DynamicCast<T>(s_LoadedAssets[info.Handle]);
		}

		
		template <typename T>
		static Shared<T> CreateMemoryAsset() 
		{
			Shared<Asset> asset = CreateShared<T>();
			s_LoadedAssets[asset->m_Handle] = asset;

			return DynamicCast<T>(s_LoadedAssets[asset->m_Handle]);
		}

		static void LoadAssetInfos();
		static void WriteAssetInfosToFile();

		static bool FileExists(const std::filesystem::path& path);

	private:
		static void OnFileSystemChanged(const std::vector<FileSystemChangeEvent>& fileSystemEvents);
		static void OnAssetRemoved(UUID assetID);
		static void OnAssetRenamed(UUID assetID, const std::filesystem::path& newFileName);
		static AssetInfo& GetAssetInfo_Internal(const UUID& assetID);
		static std::filesystem::path GetRelativePath(const std::filesystem::path& path);

	private:
		static std::unordered_map<UUID, Shared<Asset>> s_LoadedAssets;
		static AssetInfoMap s_AssetsInfos;
		static AssetChangeCallbackFn s_ChangeCallback;
	};
}
