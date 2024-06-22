#pragma once
#include "Core/UUID.h"

#include <filesystem>

namespace TerranEngine 
{
	enum class AssetType 
	{
		None = 0,
		Texture2D,
		Text,
		ScriptFile,
		Scene,
		PhysicsMaterial2D
	};

	struct AssetInfo 
	{
		AssetInfo() = default;
		~AssetInfo() = default;

		bool operator==(const AssetInfo& other) { return Path == other.Path && Type == other.Type; }
		bool operator!=(const AssetInfo& other) { return !((*this) == other); }
		operator bool() { return Path != "" && Type != AssetType::None; }

		std::filesystem::path Path = "";
		AssetType Type = AssetType::None;
		UUID Handle = UUID::Invalid();
	};

	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		bool IsValid() { return m_Handle.IsValid(); }
		const UUID& GetHandle() { return m_Handle; }
		virtual AssetType GetType() const = 0;

	protected:
		UUID m_Handle;
		friend class AssetManager;
	};

#define ASSET_CLASS_TYPE(type)\
	static AssetType GetStaticType() { return AssetType::type; }\
	virtual AssetType GetType() const override { return GetStaticType(); }

	class TextAsset : public Asset 
	{
	public:
		TextAsset(const std::string& text) 
			: m_Text(text)
		{}
		virtual ~TextAsset() override = default;

		const std::string& GetText() { return m_Text; }

		ASSET_CLASS_TYPE(Text)

	private:
		std::string m_Text;
	};

	class PhysicsMaterial2DAsset : public Asset 
	{
	public:
		PhysicsMaterial2DAsset() = default;
		virtual ~PhysicsMaterial2DAsset() override = default;

		ASSET_CLASS_TYPE(PhysicsMaterial2D)

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
	};
}
