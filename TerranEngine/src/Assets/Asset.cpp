#include "trpch.h"
#include "Asset.h"

namespace TerranEngine
{
	namespace AssetUtility 
	{
		AssetType GetAssetTypeFromFileExtenstion(const std::filesystem::path& extenstion)
		{
			if (extenstion == ".png")	return AssetType::Texture;
			if (extenstion == ".jpg")	return AssetType::Texture;
			if (extenstion == ".jpeg")	return AssetType::Texture;

			TR_ERROR("invalid asset type");
			return AssetType::None;
		}
	}
}