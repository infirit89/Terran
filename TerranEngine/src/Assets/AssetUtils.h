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
			{ ".png",		AssetType::Texture2D	},
			{ ".jpg",		AssetType::Texture2D	},
			{ ".jpeg",		AssetType::Texture2D	},
			{ ".txt",		AssetType::Text		},
			{ ".cs",		AssetType::ScriptFile	},
			{ ".terran",	AssetType::Scene	},
			{ ".trpm2d",	AssetType::PhysicsMaterial2D }
		};

		AssetType GetAssetTypeFromFileExtenstion(const std::filesystem::path& extension);
	}

}