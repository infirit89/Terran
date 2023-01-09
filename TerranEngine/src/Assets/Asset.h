#pragma once
#include "Core/UUID.h"

#include <filesystem>

namespace TerranEngine 
{
	enum class AssetType 
	{
		None = 0,
		Texture,
		Text
	};

	struct AssetInfo 
	{
		AssetInfo() = default;
		~AssetInfo() = default;

		bool operator==(const AssetInfo& other) { return Path == other.Path && Type == other.Type; }
		bool operator!=(const AssetInfo& other) { return !((*this) == other); };

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
		virtual AssetType GetType() const = 0;

	protected:
		UUID m_Handle;
		friend class AssetManager;
	};

	class TextAsset : public Asset 
	{
	public:
		TextAsset(const std::string& text) 
			: m_Text(text)
		{}
		virtual ~TextAsset() override = default;

		virtual AssetType GetType() const { return AssetType::Text; }

		const std::string& GetText() { return m_Text; }

	private:
		std::string m_Text;
	};
}