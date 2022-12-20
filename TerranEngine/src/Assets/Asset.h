#pragma once
#include "Core/UUID.h"

#include <filesystem>

namespace TerranEngine 
{
	enum class AssetType 
	{
		None = 0,
		Directory,
		Texture
	};

	namespace AssetUtility 
	{
		AssetType GetAssetTypeFromFileExtenstion(const std::filesystem::path& extenstion);
	}

	struct AssetInfo 
	{
		AssetInfo() = default;
		~AssetInfo() = default;

		std::filesystem::path Path;
		AssetType Type;
	};

	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		bool IsValid() { return m_Handle.IsValid(); }
		UUID GetHandle() { return m_Handle; }
		AssetType GetType() { return m_Type; }

	protected:
		UUID m_Handle;
		AssetType m_Type;
		friend class AssetManager;
	};
}