#pragma once

#include "Core/Base.h"
#include "Shader.h"

#include <filesystem>
#include <unordered_map>

namespace TerranEngine 
{
	class ShaderLibrary 
	{
	public:
		static void Initialize();
		
		static Shared<Shader> Load(const std::filesystem::path& shaderPath);
		static Shared<Shader> Get(const std::string& name);

	private:
		using ShaderSourcesMap = std::unordered_map<ShaderStage, std::string>;
		static ShaderSourcesMap PreprocessShaderSource(const std::string& shaderSource);
		static std::vector<ShaderUnitInfo> CompileShaderSources(const ShaderSourcesMap& shaderSources, const std::string& shaderName);

		static std::unordered_map<std::string, Shared<Shader>> s_Shaders;
	};
}
