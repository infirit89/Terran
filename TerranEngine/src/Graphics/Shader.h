#pragma once

#include <glm/mat4x4.hpp>

#include <string>
#include <unordered_map>
#include <filesystem>

namespace TerranEngine 
{
	struct ShaderUnitInfo 
	{
		uint32_t Stage;
		std::vector<uint32_t> Data;
	};

	class Shader 
	{
	public:
		Shader(const std::vector<ShaderUnitInfo>& shaderUnits);
		~Shader();

		static Shared<Shader> Create(const std::vector<ShaderUnitInfo>& shaderUnits);

		void Bind() const;
		void Unbind() const;

		void UploadInt(const char* name, int val);
		
		void UploadMat4(const char* name, const glm::mat4& val);
		void UploadIntArray(const char* name, uint32_t count, int val[]);

		inline const std::string GetName() const { return m_Name; }

	private:
		int GetUniformLocation(const char* name);
		
		void CreateProgram(const std::vector<ShaderUnitInfo>& shaderUnits);
		
	private:
		uint32_t m_Handle;
		std::string m_Name;

		std::unordered_map<const char*, uint32_t> m_Uniforms;
	};
}
