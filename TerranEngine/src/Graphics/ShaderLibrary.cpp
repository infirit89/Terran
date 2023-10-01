#include "trpch.h"
#include "ShaderLibrary.h"

#include "Utils/Utils.h"

#include <glad/glad.h>
#include <shaderc/shaderc.hpp>

namespace TerranEngine 
{
    namespace ShaderUtilities 
    {
        static ShaderStage GetShaderType(const std::string& stage)
        {
            if (stage == "vertex") return ShaderStage::Vertex;
            else if (stage == "fragment") return ShaderStage::Fragment;
            return ShaderStage::None;
        }

        static shaderc_shader_kind GetShadercShaderKind(ShaderStage stage) 
        {
            switch (stage)
            {
            case TerranEngine::ShaderStage::Vertex:     return shaderc_vertex_shader;
            case TerranEngine::ShaderStage::Fragment:   return shaderc_fragment_shader;
            }

            TR_ASSERT(false, "Invalid shader stage");
            return shaderc_vertex_shader;
        }
    }

    std::unordered_map<std::string, Shared<Shader>> ShaderLibrary::s_Shaders;

    static std::filesystem::path GetCachedShaderPath()
    {
        return "Resources/Shaders/Cache";
    }

    void ShaderLibrary::Initialize()
    {
        if (!std::filesystem::exists(GetCachedShaderPath()))
            std::filesystem::create_directory(GetCachedShaderPath());
    }

    Shared<Shader> ShaderLibrary::Load(const std::filesystem::path& shaderPath)
    {
        std::string shaderName = shaderPath.stem().string();
        Shared<Shader> shader = Get(shaderName);
        if (shader)
            return shader;

        std::ifstream ifs(shaderPath, std::ios::in | std::ios::binary);
        //TR_TRACE(, "Couldn't open shader file");

        ifs.seekg(0, std::ios::end);
        size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::string shaderSource;
        shaderSource.resize(fileSize);
        ifs.read(shaderSource.data(), fileSize);

        ShaderSourcesMap sources = PreprocessShaderSource(shaderSource);
        std::vector<ShaderUnitInfo> compiledShaders = CompileShaderSources(sources, shaderName);

        shader = CreateShared<Shader>(compiledShaders);
        s_Shaders[shader->GetName()] = shader;
        return shader;
    }
    Shared<Shader> ShaderLibrary::Get(const std::string& name)
    {
        if (s_Shaders.find(name) != s_Shaders.end())
            return s_Shaders[name];

        return nullptr;
    }

    /* TODO: add a pragma directive for the shader stage
    #version 450
    #pragma stage : vertex
    */
    ShaderLibrary::ShaderSourcesMap ShaderLibrary::PreprocessShaderSource(const std::string& shaderSource)
    {
        ShaderSourcesMap shaderSources;

        const char* shaderStageToken = "#shader_stage";
        const size_t typeTokenLength = strlen(shaderStageToken);

        size_t pos = shaderSource.find(shaderStageToken);

        while (pos != std::string::npos)
        {
            size_t eol = shaderSource.find_first_of("\r\n", pos);
            TR_ASSERT(eol != std::string::npos, "Syntax error: No new line character after the shader stage definition");

            // get the line on which describes what shader stage the following code will belong to
            std::string line = shaderSource.substr(pos, eol - pos);

            // trim the end of the line
            Utils::TrimEnd(line);

            // find the shader stage
            size_t shaderStagePos = line.find_last_of(' ');
            std::string shaderStage = line.substr(shaderStagePos + 1);
            TR_ASSERT(ShaderUtilities::GetShaderType(shaderStage) != ShaderStage::None, 
                        "Invalid shader stage specified.");

            size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
            TR_ASSERT(nextLinePos != std::string::npos, "Syntax error: Shader source for {} shader is not found", shaderStage);

            // find next #shader_stage token 
            pos = shaderSource.find(shaderStageToken, nextLinePos);

            shaderSources[ShaderUtilities::GetShaderType(shaderStage)] = shaderSource.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }
    std::vector<ShaderUnitInfo> ShaderLibrary::CompileShaderSources(const ShaderSourcesMap& shaderSources, const std::string& shaderName)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, 0);
        options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

        std::vector<ShaderUnitInfo> compiledShaders;

        for (const auto& [stage, shaderSource] : shaderSources)
        {
            shaderc::SpvCompilationResult compilationResult = 
                                            compiler.CompileGlslToSpv(
                                                shaderSource,
                                                ShaderUtilities::GetShadercShaderKind(stage),
                                                shaderName.c_str());
            if (compilationResult.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
                TR_ASSERT(false, compilationResult.GetErrorMessage());

            ShaderUnitInfo unitInfo;
            unitInfo.Stage = stage;
            unitInfo.Data = std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
            compiledShaders.emplace_back(unitInfo);
        }

        return compiledShaders;
    }
}
