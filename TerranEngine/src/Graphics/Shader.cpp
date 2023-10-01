#include "trpch.h"
#include "Shader.h"

#include "Utils/Utils.h"

#include <glad/glad.h>
#include <filesystem>

namespace TerranEngine 
{
	namespace ShaderUtilities 
	{
		static uint32_t GetOpenGLShaderType(ShaderStage stage) 
		{
			switch (stage) 
			{
			case ShaderStage::Vertex:	return GL_VERTEX_SHADER;
			case ShaderStage::Fragment:	return GL_FRAGMENT_SHADER;
			}

			TR_ASSERT(false, "Invalid shader stage");
			return 0;
		}
	}

	Shader::Shader(const std::vector<ShaderUnitInfo>& shaderUnits)
		: m_Handle(0)
	{
		CreateProgram(shaderUnits);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_Handle);
	}

	Shared<Shader> Shader::Create(const std::vector<ShaderUnitInfo>& shaderUnits)
	{
		return CreateShared<Shader>(shaderUnits);
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
		glUniform1i(GetUniformLocation(name), val);
	}

	void Shader::UploadMat4(const char* name, const glm::mat4& val)
	{
		Bind();
		glUniformMatrix4fv(GetUniformLocation(name), 1, false, &val[0][0]);
	}

	void Shader::UploadIntArray(const char* name, uint32_t count, int val[])
	{
		Bind();
		glUniform1iv(GetUniformLocation(name), count, val);
	}

	int Shader::GetUniformLocation(const char* name)
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

	void Shader::CreateProgram(const std::vector<ShaderUnitInfo>& shaderUnits)
	{
		m_Handle = glCreateProgram();

		std::vector<uint32_t> shaderIds;
		shaderIds.reserve(shaderUnits.size());
		for (const auto& shaderUnit : shaderUnits)
		{
			uint32_t shader = glCreateShader(ShaderUtilities::GetOpenGLShaderType(shaderUnit.Stage));
			glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderUnit.Data.data(),
						static_cast<int>(shaderUnit.Data.size() * sizeof(uint32_t)));
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
}
