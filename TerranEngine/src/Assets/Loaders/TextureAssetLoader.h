#pragma once

#include "Assets/AssetLoader.h"

namespace TerranEngine 
{
	class TextureAssetLoader : public AssetLoader 
	{
	public:
		virtual Shared<Asset> Load(const AssetInfo& assetInfo) override;
		virtual bool Save(const Shared<Asset>& asset) override;
	};
}