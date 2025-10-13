#pragma once

#include "LibCore/Base.h"
#include "Shader.h"

#include <filesystem>
#include <unordered_map>
#include <string>

namespace TerranEngine 
{
	class ShaderLibrary final
	{
	public:
		static void Initialize();

		static Terran::Core::Shared<Shader> Load(const std::filesystem::path& shaderPath);
		static Terran::Core::Shared<Shader> Get(const std::string& name);

		static const std::unordered_map<std::string, Terran::Core::Shared<Shader>>& GetShaders()
		{ return s_Shaders; }

	private:
		static std::unordered_map<std::string, Terran::Core::Shared<Shader>> s_Shaders;
	};
}
