#include "trpch.h"

#include "Shader.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	Shader::Shader()
		: m_SProgram(0), m_IsProgramBound(false)
#ifdef TR_DEBUG
		, m_VertexPath(nullptr), m_FragmentPath(nullptr), m_ShaderPath(nullptr)
#endif
	{

	}

	Shader::Shader(const char* shaderPath)
		: m_SProgram(0), m_IsProgramBound(false)

#ifdef TR_DEBUG
		, m_ShaderPath(shaderPath), m_VertexPath(nullptr), m_FragmentPath(nullptr)
#endif

	{
		FileData* data = File::OpenFile(shaderPath);
		
		auto& shaderSources = ProcessShaderFile(data->Data);
		CreateProgram(shaderSources);
		Bind();

		File::CloseFile(data);
	}

	Shader::Shader(const char* vertexPath, const char* fragmentPath)
		: m_SProgram(0), m_IsProgramBound(false)

#ifdef TR_DEBUG
		, m_VertexPath(vertexPath), m_FragmentPath(fragmentPath), m_ShaderPath(nullptr)
#endif

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

	std::unordered_map<uint32_t, std::string> Shader::ProcessShaderFile(const char* shaderSource)
	{
		std::string fileSource = shaderSource;

		std::unordered_map<uint32_t, std::string> shaderSources;

		const char* typeToken = "#type";
		const size_t typeTokenLength = strlen(typeToken);

		size_t pos = fileSource.find(typeToken);
		while (pos != std::string::npos)
		{
			//find end of line
			size_t eol = fileSource.find("\r\n", pos);
			TR_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = fileSource.substr(begin, eol - begin);
			TR_ASSERT(GetShaderType(type), "Invalid shader type specified.");

			size_t nextLinePos = fileSource.find_first_not_of("\r\n", eol);
			TR_ASSERT(nextLinePos != std::string::npos, "Syntax error");

			pos = fileSource.find(typeToken, nextLinePos);

			shaderSources[GetShaderType(type)] = fileSource.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void Shader::CreateProgram(std::unordered_map<uint32_t, std::string>& shaderSources)
	{
		m_SProgram = glCreateProgram();

		unsigned int vertShader = CreateShader(shaderSources[GL_VERTEX_SHADER].c_str(), GL_VERTEX_SHADER);
		unsigned int fragShader = CreateShader(shaderSources[GL_FRAGMENT_SHADER].c_str(), GL_FRAGMENT_SHADER);

		glAttachShader(m_SProgram, vertShader);
		glAttachShader(m_SProgram, fragShader);

		glLinkProgram(m_SProgram);
		glValidateProgram(m_SProgram);

		glDetachShader(m_SProgram, vertShader);
		glDetachShader(m_SProgram, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
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

	uint32_t Shader::GetShaderType(std::string& typeStr)
	{
		if (typeStr == "vertex")
			return GL_VERTEX_SHADER;
		else if (typeStr == "fragment")
			return GL_FRAGMENT_SHADER;
		return 0;
	}
}
