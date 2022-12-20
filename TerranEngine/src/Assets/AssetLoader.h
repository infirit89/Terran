#pragma once

#include "Core/Base.h"

#include "Asset.h"

namespace TerranEngine 
{
	class AssetLoader 
	{
	public:
		virtual Shared<Asset> Load(const AssetInfo& assetInfo) = 0;
		virtual bool Save(const Shared<Asset>& asset) = 0;
	};
}