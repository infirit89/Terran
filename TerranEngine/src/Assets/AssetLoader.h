#pragma once

#include "Core/Base.h"

#include "Asset.h"

namespace TerranEngine 
{
	class AssetLoader 
	{
	public:
		virtual void Load(const AssetInfo& assetInfo, Shared<Asset>& asset) = 0;
		virtual bool Save(const AssetInfo& assetInfo, const Shared<Asset>& asset) = 0;
	};
}