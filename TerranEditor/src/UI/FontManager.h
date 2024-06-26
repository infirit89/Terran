#pragma once

#include <string>
#include <filesystem>

#include <imgui.h>

namespace TerranEditor 
{
	namespace UI 
	{
		struct FontConfig 
		{
			std::string Name;
			std::filesystem::path Path;
			float Size = 16.0f;
			bool MergeMode = false;
			bool PixelSnapH = false;
			float GlyphMixAdvanceX = 0.0f;
			const ImWchar* GlyphRange = nullptr;
		};

		class FontManager 
		{
		public:
			static void AddFont(const FontConfig& config, bool isDefault = false);
			static ImFont* GetFont(const std::string& name);
			static void BuildFontAtlas();
			static void PushFont(const std::string& name);
			static void PopFont();
		};
	}
}
