#include "trpch.h"

#include "ShaderLibrary.h"

#include "ShaderCompiler.h"

namespace TerranEngine {

std::unordered_map<std::string, Terran::Core::Shared<Shader>> ShaderLibrary::s_Shaders;

void ShaderLibrary::Initialize()
{
    if (!std::filesystem::exists(GetCachedShaderPath()))
        std::filesystem::create_directory(GetCachedShaderPath());
}

Terran::Core::Shared<Shader> ShaderLibrary::Load(std::filesystem::path const& shaderPath)
{
    Terran::Core::Shared<ShaderCompiler> compiler = Terran::Core::CreateShared<ShaderCompiler>(shaderPath);
    Terran::Core::Shared<Shader> shader = compiler->Compile();
    s_Shaders[shader->GetName()] = shader;
    return shader;
}

Terran::Core::Shared<Shader> ShaderLibrary::Get(std::string const& name)
{
    if (s_Shaders.contains(name))
        return s_Shaders.at(name);

    return nullptr;
}

}
