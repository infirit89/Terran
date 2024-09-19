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
		static void Initialize();
		static void Shutdown();

		static const AssetInfo& GetAssetInfoByHandle(AssetHandle assetHandle);
		static const AssetInfo& GetAssetInfoByPath(const std::filesystem::path& assetPath);
		static AssetHandle GetAssetHandleFromPath(const std::filesystem::path& assetPath);

		static std::filesystem::path GetFileSystemPath(const std::filesystem::path& path);

		static AssetInfoMap& GetAssetInfoMap() { return s_AssetsInfos; }

		static AssetHandle ImportAsset(const std::filesystem::path& assetPath);
		static void ReloadAssetByHandle(AssetHandle assetHandle);

		inline static void SetAssetChangedCallback(AssetChangeCallbackFn callback) { s_ChangeCallback = callback; }

		template<typename T>
		inline static Shared<T> GetAssetByHandle(const AssetHandle& assetHandle)
		{
			if (s_LoadedAssets.find(assetHandle) != s_LoadedAssets.end())
				return DynamicCast<T>(s_LoadedAssets.at(assetHandle));

			AssetInfo& info = GetAssetInfoByHandle_Internal(assetHandle);

			if (!info)
				return nullptr;

			// NOTE: poc code
			Shared<Asset> asset = nullptr;
			AssetImporter::Load(info, asset);
				
			if (!asset)
			{
				TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", info.Path);
				return nullptr;
			}

			asset->m_Handle = assetHandle;
			s_LoadedAssets[assetHandle] = asset;
			return DynamicCast<T>(s_LoadedAssets[assetHandle]);
		}

		template<typename T>
		inline static Shared<T> GetAssetByAssetInfo(const AssetInfo& assetInfo) 
		{
			if (s_LoadedAssets.contains(assetInfo.Handle))
				return DynamicCast<T>(s_LoadedAssets.at(assetInfo.Handle));

			// NOTE: poc code
			Shared<Asset> asset = nullptr;
			AssetImporter::Load(assetInfo, asset);

			if (!asset)
			{
				TR_CORE_ERROR(TR_LOG_ASSET, "Failed to load asset with path: {0}", assetInfo.Path);
				return nullptr;
			}

			asset->m_Handle = assetInfo.Handle;
			s_LoadedAssets[assetInfo.Handle] = asset;
			return DynamicCast<T>(s_LoadedAssets[assetInfo.Handle]);
		}

		template<typename T>
		static Shared<T> CreateNewAsset(const std::filesystem::path& filePath) 
		{
			AssetInfo info;
			info.Handle = AssetHandle();
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
		static void OnAssetRemoved(AssetHandle assetID);
		static void OnAssetRenamed(AssetHandle assetID, const std::filesystem::path& newFileName);
		static AssetInfo& GetAssetInfoByHandle_Internal(const AssetHandle& assetID);
		static std::filesystem::path GetRelativePath(const std::filesystem::path& path);

	private:
		static std::unordered_map<AssetHandle, Shared<Asset>> s_LoadedAssets;
		static AssetInfoMap s_AssetsInfos;
		static AssetChangeCallbackFn s_ChangeCallback;
	};
}
