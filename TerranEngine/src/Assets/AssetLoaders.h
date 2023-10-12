#pragma once

#include "Core/Base.h"

#include "Asset.h"

#include "Graphics/Texture.h"

namespace TerranEngine 
{
	class AssetLoader
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) = 0;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) = 0;
	};

	class TextureAssetLoader : public AssetLoader
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) override;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) override;
		static Shared<Texture2D> CreateTextureFromFile(const std::filesystem::path& textureSourcePath);
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

	class PhysicsMaterial2DAssetLoader : public AssetLoader 
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) override;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) override;
	};
}