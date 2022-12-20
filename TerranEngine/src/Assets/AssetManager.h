#pragma once

#include "Asset.h"
#include "Loaders/TextureAssetLoader.h"

#include "Core/Base.h"

#include <unordered_map>
#include <filesystem>

namespace TerranEngine 
{
	class AssetManager 
	{
		using AssetInfoMap = std::unordered_map<UUID, AssetInfo>;

	public:
		static void Init();
		static void Shutdown();

		static AssetInfo& GetAssetInfo(const UUID& assetID);

		static AssetInfoMap& GetAssetInfoMap() { return s_AssetsInfos; }

		static UUID ImportAsset(const std::filesystem::path& assetPath);

		template<typename T>
		static Shared<T> GetAsset(const UUID& assetID) 
		{
			if (s_LoadedAssets.find(assetID) != s_LoadedAssets.end())
				return DynamicCast<T>(s_LoadedAssets.at(assetID));
			else 
			{
				AssetInfo& info = GetAssetInfo(assetID);

				// NOTE: poc code
				TextureAssetLoader testLoader;
				Shared<Asset> asset = testLoader.Load(info);

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
		static void WriteAssetInfos();

	private:
		static std::unordered_map<UUID, Shared<Asset>> s_LoadedAssets;
		static AssetInfoMap s_AssetsInfos;
	};
}