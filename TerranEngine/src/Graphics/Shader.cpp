#include "trpch.h"
#include "Shader.h"

#include "Utils/Utils.h"

#include <glad/glad.h>
#include <filesystem>
#include <shaderc/shaderc.hpp>

namespace TerranEngine 
{
	namespace ShaderUtilities 
	{
		static uint32_t GetShaderType(std::string& typeStr)
		{
			if (typeStr == "vertex")
				return GL_VERTEX_SHADER;
			else if (typeStr == "fragment")
				return GL_FRAGMENT_SHADER;
			return 0;
		}

		static shaderc_shader_kind GetShadercShaderKind(uint32_t openglShaderType) 
		{
			switch (openglShaderType)
			{
			case GL_VERTEX_SHADER: return shaderc_shader_kind::shaderc_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_shader_kind::shaderc_fragment_shader;
			}

			TR_ASSERT(false, "Unknown shader type");
			return shaderc_shader_kind::shaderc_glsl_infer_from_source;
		}

		static std::unordered_map<uint32_t, std::string> PrerocessShaderSource(const std::string& shaderSource)
		{
			std::unordered_map<uint32_t, std::string> shaderSources;

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
				TR_ASSERT(GetShaderType(shaderStage), "Invalid shader stage specified.");

				size_t nextLinePos = shaderSource.find_first_not_of("\r\n", eol);
				TR_ASSERT(nextLinePos != std::string::npos, "Syntax error: Shader source for {} shader is not found", shaderStage);

				// find next #shader_stage token 
				pos = shaderSource.find(shaderStageToken, nextLinePos);

				shaderSources[GetShaderType(shaderStage)] = shaderSource.substr(nextLinePos, pos - nextLinePos);
			}

			return shaderSources;
		}
	}

	Shader::Shader(const std::filesystem::path& shaderPath)
		: m_Handle(0)
	{
		m_Name = shaderPath.stem().string();

		// use the c++ std dipshit
		FileData* data = File::OpenFile(shaderPath.string().c_str());
		
		auto shaderSources = ShaderUtilities::PrerocessShaderSource(data->Data);
		auto compiledShaders = CompileShaders(shaderSources);
		CreateProgram(compiledShaders);

		File::CloseFile(data);
	}

	/*Shader::Shader(const char* name, const char* vertexPath, const char* fragmentPath)
		: m_Handle(0)

#ifdef TR_DEBUG
		, m_VertexPath(vertexPath), m_FragmentPath(fragmentPath), m_ShaderPath(nullptr)
#endif

	{
		FileData* vertexFile = File::OpenFile(vertexPath);
		FileData* fragmentFile = File::OpenFile(fragmentPath);

		CreateProgram(vertexFile->Data, fragmentFile->Data);

		File::CloseFile(vertexFile);
		File::CloseFile(fragmentFile);
	}*/

	Shader::~Shader()
	{
		glDeleteProgram(m_Handle);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_Handle);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::UploadInt(const char* name, int val)
	{
		Bind();
		glUniform1i(GetUniformLoc(name), val);
	}

	void Shader::UploadInt2(const char* name, int val1, int val2)
	{
		Bind();
		glUniform2i(GetUniformLoc(name), val1, val2);
	}

	void Shader::UploadInt3(const char* name, int val1, int val2, int val3)
	{
		Bind();
		glUniform3i(GetUniformLoc(name), val1, val2, val3);
	}

	void Shader::UploadInt4(const char* name, int val1, int val2, int val3, int val4)
	{
		Bind();
		glUniform4i(GetUniformLoc(name), val1, val2, val3, val4);
	}

	void Shader::UploadFloat(const char* name, float val)
	{
		Bind();
		glUniform1f(GetUniformLoc(name), val);
	}

	void Shader::UploadFloat2(const char* name, float val1, float val2)
	{
		Bind();
		glUniform2f(GetUniformLoc(name), val1, val2);
	}

	void Shader::UploadFloat3(const char* name, float val1, float val2, float val3)
	{
		Bind();
		glUniform3f(GetUniformLoc(name), val1, val2, val3);
	}

	void Shader::UploadFloat4(const char* name, float val1, float val2, float val3, float val4)
	{
		Bind();
		glUniform4f(GetUniformLoc(name), val1, val2, val3, val4);
	}

	void Shader::UploadMat4(const char* name, glm::mat4x4 val)
	{
		Bind();
		glUniformMatrix4fv(GetUniformLoc(name), 1, false, &val[0][0]);
	}

	void Shader::UploadIntArray(const char* name, uint32_t count, int val[])
	{
		Bind();
		glUniform1iv(GetUniformLoc(name), count, val);
	}

	int Shader::GetUniformLoc(const char* name)
	{
		if (m_Uniforms.find(name) != m_Uniforms.end()) 
			return m_Uniforms[name];

		int loc = glGetUniformLocation(m_Handle, name);

		if (loc == -1)
		{
			TR_ASSERT(false, "Couldn't find a uniform variable with the name {0}", name);
			return 0;
		}

		m_Uniforms.insert({ name, loc });

		return loc;
	}

	uint32_t Shader::CreateShader(const char* source, unsigned int type)
	{
		uint32_t shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)malloc(length * sizeof(char));

			glGetShaderInfoLog(shader, length, &length, message);

			TR_ASSERT(false, message);

			free(message);

			return 0;
		}

		return shader;
	}

	void Shader::CreateProgram(std::unordered_map<uint32_t, std::vector<uint32_t>>& compiledShaders)
	{
		m_Handle = glCreateProgram();

		std::vector<uint32_t> shaderIds;
		shaderIds.reserve(compiledShaders.size());
		for (const auto& [stage, shaderBinary] : compiledShaders) 
		{
			uint32_t shader = glCreateShader(stage);
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary.data(), shaderBinary.size() * sizeof(uint32_t));
			glSpecializeShader(shader, "main", 0, nullptr, nullptr);
			glAttachShader(m_Handle, shader);
			shaderIds.emplace_back(shader);
		}

		glLinkProgram(m_Handle);
		int linkResult;
		glGetProgramiv(m_Handle, GL_LINK_STATUS, &linkResult);

		if (linkResult == GL_FALSE) 
		{
			int length = 0;
			glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &length);
			std::string infoLog;
			infoLog.resize(length);

			glGetProgramInfoLog(m_Handle, length, &length, &infoLog[0]);

			glDeleteProgram(m_Handle);

			TR_ASSERT(false, infoLog);
		}

		glValidateProgram(m_Handle);

		for (const auto& shaderId : shaderIds) 
		{
			glDetachShader(m_Handle, shaderId);
			glDeleteShader(shaderId);
		}
	}

	void Shader::CreateProgram(const char* vertexSource, const char* fragmentSource)
	{
		m_Handle = glCreateProgram();

		uint32_t vertShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
		uint32_t fragShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

		glAttachShader(m_Handle, vertShader);
		glAttachShader(m_Handle, fragShader);

		glLinkProgram(m_Handle);
		glValidateProgram(m_Handle);

		glDetachShader(m_Handle, vertShader);
		glDetachShader(m_Handle, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}

	// NOTE: in the runtime the shader compiler shouldn't be included;
	// shaders should be read from cached binary files
	std::unordered_map<uint32_t, std::vector<uint32_t>> Shader::CompileShaders(const std::unordered_map<uint32_t, std::string>& shaderSources)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, 0);
		options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		std::unordered_map<uint32_t, std::vector<uint32_t>> compiledShaders;

		for (const auto& [stage, shaderSource] : shaderSources)
		{
			shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(shaderSource, ShaderUtilities::GetShadercShaderKind(stage), m_Name.c_str());
			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
				TR_ASSERT(false, compilationResult.GetErrorMessage());

			compiledShaders[stage] = std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
		}

		return compiledShaders;
	}
}
