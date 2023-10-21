#include "FontManager.h"

#include <unordered_map>

namespace TerranEditor 
{
	namespace UI
	{
		std::unordered_map<std::string, ImFont*> s_FontMap;

		void FontManager::AddFont(const FontConfig& config, bool isDefault)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFontConfig imFontConfig;
			imFontConfig.MergeMode = config.MergeMode;
			imFontConfig.PixelSnapH = config.PixelSnapH;
			imFontConfig.GlyphMinAdvanceX = config.GlyphMixAdvanceX;

			ImFont* font = io.Fonts->AddFontFromFileTTF(config.Path.string().c_str(),
														config.Size, &imFontConfig,
														config.GlyphRange);
			s_FontMap.emplace(config.Path.stem().string(), font);

			if (isDefault)
				io.FontDefault = font;
		}

		ImFont* FontManager::GetFont(const std::string& name)
		{
			if (s_FontMap.find(name) != s_FontMap.end())
				return s_FontMap.at(name);

			return nullptr;
		}

		void FontManager::BuildFontAtlas()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->Build();
		}
	}
}

