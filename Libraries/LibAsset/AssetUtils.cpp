#include "trpch.h"
#include "AssetUtils.h"

namespace TerranEngine
{
	namespace AssetUtility
	{
		AssetType GetAssetTypeFromFileExtenstion(const std::filesystem::path& extension)
		{
			if (s_FileExtensionMap.find(extension) != s_FileExtensionMap.end())
				return s_FileExtensionMap.at(extension);

			return AssetType::None;
		}
	}
}