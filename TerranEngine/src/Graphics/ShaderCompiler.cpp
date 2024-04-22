#include "trpch.h"
#include "ShaderCompiler.h"

#include "Utils/Utils.h"

#include <shaderc/shaderc.hpp>

namespace TerranEngine 
{
    namespace Utilities 
    {
        static std::string GetCachedShaderExtension(ShaderStage stage)
        {
            switch (stage)
            {
            case TerranEngine::ShaderStage::Vertex:     return ".vert.cached";
            case TerranEngine::ShaderStage::Fragment:   return ".frag.cached";
            }

            TR_ASSERT(false, "Invalid shader stage");
            return "";
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

    ShaderCompiler::ShaderCompiler(const std::filesystem::path& shaderPath)
        : m_ShaderPath(shaderPath) { }

    Shared<Shader> ShaderCompiler::Compile()
    {
        std::string shaderName = m_ShaderPath.stem().string();
        std::string shaderSource = ReadShader();

        TR_CORE_INFO(TR_LOG_RENDERER, "Compiling or loading cached shader: {0}", shaderName);
        ShaderSourcesMap sources = Preprocess(shaderSource);
        std::vector<ShaderUnitInfo> compiledShaders = Compile_Internal(sources, shaderName);

        Shared<Shader> shader = CreateShared<Shader>(compiledShaders);
        shader->m_Name = shaderName;
        shader->m_Path = m_ShaderPath;
        TR_CORE_INFO(TR_LOG_RENDERER, "Compiling or loading done!");

        return shader;
    }

    void ShaderCompiler::Recompile(Shared<Shader> shader)
    {
        Shared<ShaderCompiler> compiler = CreateShared<ShaderCompiler>(shader->m_Path);
        std::string shaderSource = compiler->ReadShader();

        TR_CORE_INFO(TR_LOG_RENDERER, "Recompiling shader: {0}", shader->GetName());
        ShaderSourcesMap sources = compiler->Preprocess(shaderSource);
        for (const auto& [stage, shaderSources] : sources)
        {
            std::filesystem::path cachedShaderPath = GetCachedShaderPath() / (shader->GetName() + Utilities::GetCachedShaderExtension(stage));
            if (std::filesystem::exists(cachedShaderPath))
                std::filesystem::remove(cachedShaderPath);
        }

        std::vector<ShaderUnitInfo> compiledShaders = compiler->Compile_Internal(sources, shader->GetName());
        shader->Release();
        shader->CreateProgram(compiledShaders);
        TR_CORE_INFO(TR_LOG_RENDERER, "Compiling or loading done!");
    }

    std::vector<ShaderUnitInfo> ShaderCompiler::Compile_Internal(const ShaderSourcesMap& shaderSources, const std::string& shaderName)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_opengl, 0);
        options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

        std::vector<ShaderUnitInfo> compiledShaders;

        for (const auto& [stage, shaderSource] : shaderSources)
        {
            std::filesystem::path cachedShaderPath = GetCachedShaderPath() / (shaderName + Utilities::GetCachedShaderExtension(stage));
            if (std::filesystem::exists(cachedShaderPath))
            {
                std::ifstream inputFileStream(cachedShaderPath, std::ios::in | std::ios::binary);

                inputFileStream.seekg(0, std::ios::end);
                size_t shaderFileSize = inputFileStream.tellg();
                inputFileStream.seekg(0, std::ios::beg);

                ShaderUnitInfo unitInfo;
                unitInfo.Stage = stage;
                unitInfo.Data.resize(shaderFileSize / sizeof(uint32_t));

                inputFileStream.read((char*)unitInfo.Data.data(), shaderFileSize);
                compiledShaders.emplace_back(unitInfo);
            }
            else
            {
                shaderc::SpvCompilationResult compilationResult =
                    compiler.CompileGlslToSpv(
                        shaderSource,
                        Utilities::GetShadercShaderKind(stage),
                        shaderName.c_str());
                if (compilationResult.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
                    TR_ASSERT(false, compilationResult.GetErrorMessage());

                ShaderUnitInfo unitInfo;
                unitInfo.Stage = stage;
                unitInfo.Data = std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
                compiledShaders.emplace_back(unitInfo);

                std::ofstream ouputFileStream(cachedShaderPath, std::ios::out | std::ios::binary);
                ouputFileStream.write((char*)unitInfo.Data.data(), unitInfo.Data.size() * sizeof(uint32_t));
            }
        }

        return compiledShaders;
    }

    /* TODO: add a pragma directive for the shader stage
    #version 450
    #pragma stage : vertex
    */
    ShaderCompiler::ShaderSourcesMap ShaderCompiler::Preprocess(const std::string& shaderSource)
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
            TR_ASSERT(GetStageStageFromString(shaderStage) != ShaderStage::None,
                "Invalid shader stage specified.");

            size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
            TR_ASSERT(nextLinePos != std::string::npos, "Syntax error: Shader source for {} shader is not found", shaderStage);

            // find next #shader_stage token 
            pos = shaderSource.find(shaderStageToken, nextLinePos);

            shaderSources[GetStageStageFromString(shaderStage)] = shaderSource.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    std::string ShaderCompiler::ReadShader()
    {
        std::ifstream ifs(m_ShaderPath, std::ios::in | std::ios::binary);
        TR_ASSERT(ifs, "Couldn't open shader file");

        ifs.seekg(0, std::ios::end);
        size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::string shaderSource;
        shaderSource.resize(fileSize);
        ifs.read(shaderSource.data(), fileSize);
        return shaderSource;
    }
}
