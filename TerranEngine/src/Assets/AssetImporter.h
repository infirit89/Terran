#pragma once

#include "AssetLoaders.h"

#include <unordered_map>

namespace TerranEngine 
{
	class AssetImporter 
	{
	public:
		static void RegisterLoaders();

		static void Load(const AssetInfo& assetInfo, Shared<Asset>& asset);
		static bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset);

	private:
		static std::unordered_map<AssetType, Shared<AssetLoader>> s_Loaders;
	};
}