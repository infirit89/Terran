#pragma once

#include "AssetLoader.h"

namespace TerranEngine 
{
	class TextureAssetLoader : public AssetLoader
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) override;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) override;
	};

	class TextAssetLoader : public AssetLoader 
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) override;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) override;
	};

	class SceneAssetLoader : public AssetLoader 
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) override;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) override;
	};
}