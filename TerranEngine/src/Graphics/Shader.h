#pragma once

#include <glm/mat4x4.hpp>

#include <string>
#include <unordered_map>
#include <filesystem>

namespace TerranEngine 
{
	class Shader 
	{
	public:
		Shader(const std::filesystem::path& shaderPath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadInt(const char* name, int val);
		void UploadInt2(const char* name, int val1, int val2);
		void UploadInt3(const char* name, int val1, int val2, int val3);
		void UploadInt4(const char* name, int val1, int val2, int val3, int val4);

		void UploadFloat(const char* name, float val);
		void UploadFloat2(const char* name, float val1, float val2);
		void UploadFloat3(const char* name, float val1, float val2, float val3);
		void UploadFloat4(const char* name, float val1, float val2, float val3, float val4);

		void UploadMat4(const char* name, glm::mat4x4 val);
		void UploadIntArray(const char* name, uint32_t count, int val[]);

		inline const std::string GetName() const { return m_Name; }

	private:
		int GetUniformLoc(const char* name);
		uint32_t CreateShader(const char* source, unsigned int type);

		void CreateProgram(std::unordered_map<uint32_t, std::vector<uint32_t>>& compiledShaders);
		void CreateProgram(const char* vertexSource, const char* fragmentSource);

		std::unordered_map<uint32_t, std::vector<uint32_t>> CompileShaders(const std::unordered_map<uint32_t, std::string>& shaders);

	private:
		uint32_t m_Handle;
		std::string m_Name;

		std::unordered_map<const char*, uint32_t> m_Uniforms;
	};
}
