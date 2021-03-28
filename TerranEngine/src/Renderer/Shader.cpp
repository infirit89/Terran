#include "trpch.h"

#include "Shader.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	Shader::Shader()
		: m_SProgram(0), m_IsProgramBound(false)
	{

	}

	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
		: m_SProgram(0), m_IsProgramBound(false)
	{
		std::string vertSource = LoadShader(vertexPath);
		std::string fragSource = LoadShader(fragmentPath);

		CreateProgram(vertSource, fragSource);
		Bind();
	}

	Shader::~Shader()
	{
		m_IsProgramBound = false;
		glDeleteProgram(m_SProgram);
	}

	void Shader::Bind() const
	{
		if (!m_IsProgramBound)
		{
			m_IsProgramBound = true;
			glUseProgram(m_SProgram);
		}
	}

	void Shader::Unbind() const
	{
		m_IsProgramBound = false;
		glUseProgram(0);
	}

	void Shader::UploadInt(const std::string& name, int val)
	{
		Bind();
		glUniform1i(GetUniformLoc(name), val);
	}

	void Shader::UploadInt2(const std::string& name, int val1, int val2)
	{
		Bind();
		glUniform2i(GetUniformLoc(name), val1, val2);
	}

	void Shader::UploadInt3(const std::string& name, int val1, int val2, int val3)
	{
		Bind();
		glUniform3i(GetUniformLoc(name), val1, val2, val3);
	}

	void Shader::UploadInt4(const std::string& name, int val1, int val2, int val3, int val4)
	{
		Bind();
		glUniform4i(GetUniformLoc(name), val1, val2, val3, val4);
	}

	void Shader::UploadFloat(const std::string& name, float val)
	{
		Bind();
		glUniform1f(GetUniformLoc(name), val);
	}

	void Shader::UploadFloat2(const std::string& name, float val1, float val2)
	{
		Bind();
		glUniform2f(GetUniformLoc(name), val1, val2);
	}

	void Shader::UploadFloat3(const std::string& name, float val1, float val2, float val3)
	{
		Bind();
		glUniform3f(GetUniformLoc(name), val1, val2, val3);
	}

	void Shader::UploadFloat4(const std::string& name, float val1, float val2, float val3, float val4)
	{
		Bind();
		glUniform4f(GetUniformLoc(name), val1, val2, val3, val4);
	}

	int Shader::GetUniformLoc(const std::string& name)
	{
		int loc = glGetUniformLocation(m_SProgram, name.c_str());

		if (loc == -1)
		{
			TR_ASSERTMSG(false, "Couldn't find a uniform variable with the name {0}", name);
			return 0;
		}

		return loc;
	}

	std::string Shader::LoadShader(const std::string& filePath)
	{
		std::string result;

		std::ifstream is(filePath, std::ifstream::binary);

		if (is)
		{
			is.seekg(0, is.end);
			int length = is.tellg();
			if (length != -1)
			{
				is.seekg(0, is.beg);
				result.resize(length);
				is.read(&result[0], length);
			}
			else
				TR_ERROR("Couldn't read from file " + filePath);
		}
		else
			TR_ERROR("Couldn't oppen file " +  filePath);

		is.close();

		return result;
	}

	int Shader::CreateShader(const std::string& source, unsigned int type)
	{
		unsigned int shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);

		int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) 
		{
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));

			glGetShaderInfoLog(shader, length, &length, message);

			TR_ASSERTMSG(false, message);
			return 0;
		}

		return shader;
	}

	void Shader::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource)
	{
		m_SProgram = glCreateProgram();

		unsigned int vertShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
		unsigned int fragShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

		glAttachShader(m_SProgram, vertShader);
		glAttachShader(m_SProgram, fragShader);

		glLinkProgram(m_SProgram);
		glValidateProgram(m_SProgram);

		glDetachShader(m_SProgram, vertShader);
		glDetachShader(m_SProgram, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
	}
}
