#include "trpch.h"

#include "Shader.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	Shader::Shader()
		: m_SProgram(0), m_IsProgramBound(false)
	{

	}

	Shader::Shader(const char* vertexPath, const char* fragmentPath)
		: m_SProgram(0), m_IsProgramBound(false), m_VertexPath(vertexPath), m_FragmentPath(fragmentPath)
	{
		FileData* vertexFile = File::OpenFile(vertexPath);
		FileData* fragmentFile = File::OpenFile(fragmentPath);

		CreateProgram(vertexFile->Data, fragmentFile->Data);
		Bind();

		File::CloseFile(vertexFile);
		File::CloseFile(fragmentFile);
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

		int loc = glGetUniformLocation(m_SProgram, name);

		if (loc == -1)
		{
			TR_ASSERT(false, "Couldn't find a uniform variable with the name {0}", name);
			return 0;
		}

		m_Uniforms.insert({ name, loc });

		return loc;
	}

	int Shader::CreateShader(const char* source, unsigned int type)
	{
		unsigned int shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);

		int result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) 
		{
			int length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)alloca(length * sizeof(char));

			glGetShaderInfoLog(shader, length, &length, message);

			TR_ASSERT(false, message);
			return 0;
		}

		return shader;
	}

	void Shader::CreateProgram(const char* vertexSource, const char* fragmentSource)
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
