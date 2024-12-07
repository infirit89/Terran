#pragma once

#include "Core/Assert.h"

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

namespace TerranEngine 
{
	enum class ShaderStage : uint8_t
	{
		None = 0,
		Vertex,
		Fragment
	};

	static ShaderStage GetStageStageFromString(const std::string& stageStr) 
	{
		if (stageStr == "vertex")	return ShaderStage::Vertex;
		if (stageStr == "fragment")	return ShaderStage::Fragment;

		TR_ASSERT(false, "Invalid shader stage");
		return ShaderStage::None;
	}

	static std::filesystem::path GetCachedShaderPath()
	{
		return "Resources/Shaders/Cache";
	}

	struct ShaderUnitInfo final
	{
		ShaderStage Stage;
		std::vector<uint32_t> Data;
	};
}
