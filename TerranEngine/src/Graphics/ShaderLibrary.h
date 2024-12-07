#pragma once

#include "Core/Base.h"
#include "Shader.h"

#include <filesystem>
#include <unordered_map>

namespace TerranEngine 
{
	class ShaderLibrary final
	{
	public:
		static void Initialize();

		static Shared<Shader> Load(const std::filesystem::path& shaderPath);
		static Shared<Shader> Get(const std::string& name);

		static const std::unordered_map<std::string, Shared<Shader>>& GetShaders()
		{ return s_Shaders; }

	private:
		static std::unordered_map<std::string, Shared<Shader>> s_Shaders;
	};
}
