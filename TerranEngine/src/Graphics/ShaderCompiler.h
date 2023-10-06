#pragma once

#include "Shader.h"

#include <filesystem>

namespace TerranEngine 
{
	class ShaderCompiler 
	{
	public:
		ShaderCompiler(const std::filesystem::path& shaderPath);
		Shared<Shader> Compile();

	private:
		using ShaderSourcesMap = std::unordered_map<ShaderStage, std::string>;
		std::vector<ShaderUnitInfo> Compile_Internal(const ShaderSourcesMap& shaderSources, const std::string& shaderName);
		ShaderSourcesMap Preprocess(const std::string& shaderSource);

		std::filesystem::path m_ShaderPath;
	};
}
