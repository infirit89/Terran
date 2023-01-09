#pragma once

#include "Asset.h"

#include <unordered_map>
#include <filesystem>

//namespace std
//{
//	template<>
//	struct hash<std::filesystem::path>
//	{
//		size_t operator() (const std::filesystem::path& path) 
//		{
//			hash<string> tempHash;
//			return tempHash(path.string());
//		}
//	};
//}

namespace TerranEngine 
{
	

	namespace AssetUtility 
	{
		inline std::unordered_map<std::filesystem::path, AssetType> s_FileExtensionMap = 
		{
			{ ".png",  AssetType::Texture },
			{ ".jpg",  AssetType::Texture },
			{ ".jpeg", AssetType::Texture },
			{ ".txt",  AssetType::Text	  }
		};

		AssetType GetAssetTypeFromFileExtenstion(const std::filesystem::path& extension);
	}

}