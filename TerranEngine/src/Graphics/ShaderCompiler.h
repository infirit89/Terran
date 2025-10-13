#pragma once

#include "Shader.h"

#include <filesystem>

namespace TerranEngine {

class ShaderCompiler final {
public:
    ShaderCompiler(std::filesystem::path const& shaderPath);
    Terran::Core::Shared<Shader> Compile();
    static void Recompile(Terran::Core::Shared<Shader> const& shader);

private:
    using ShaderSourcesMap = std::unordered_map<ShaderStage, std::string>;
    std::vector<ShaderUnitInfo> Compile_Internal(ShaderSourcesMap const& shaderSources, std::string const& shaderName);
    ShaderSourcesMap Preprocess(std::string const& shaderSource);
    std::string ReadShader() const;

    std::filesystem::path m_ShaderPath;
};

}
