#include "trpch.h"
#include "ShaderLibrary.h"

#include "ShaderCompiler.h"

namespace TerranEngine 
{
    std::unordered_map<std::string, Shared<Shader>> ShaderLibrary::s_Shaders;

    void ShaderLibrary::Initialize()
    {
        if (!std::filesystem::exists(GetCachedShaderPath()))
            std::filesystem::create_directory(GetCachedShaderPath());
    }

    Shared<Shader> ShaderLibrary::Load(const std::filesystem::path& shaderPath)
    {
        Shared<ShaderCompiler> compiler = CreateShared<ShaderCompiler>(shaderPath);
        Shared<Shader> shader = compiler->Compile();
        s_Shaders[shader->GetName()] = shader;
        return shader;
    }
    Shared<Shader> ShaderLibrary::Get(const std::string& name)
    {
        if (s_Shaders.find(name) != s_Shaders.end())
            return s_Shaders[name];

        return nullptr;
    }
}
