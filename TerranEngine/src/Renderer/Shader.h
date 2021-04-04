#pragma once

#include <string>
#include <glm/mat4x4.hpp>

namespace TerranEngine 
{
	class Shader 
	{
	public:
		Shader();
		Shader(const std::string& vertexPath, const std::string& fragmentPath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadInt(const std::string& name, int val);
		void UploadInt2(const std::string& name, int val1, int val2);
		void UploadInt3(const std::string& name, int val1, int val2, int val3);
		void UploadInt4(const std::string& name, int val1, int val2, int val3, int val4);

		void UploadFloat(const std::string& name, float val);
		void UploadFloat2(const std::string& name, float val1, float val2);
		void UploadFloat3(const std::string& name, float val1, float val2, float val3);
		void UploadFloat4(const std::string& name, float val1, float val2, float val3, float val4);

		void UploadMat4(const std::string& name, glm::mat4x4 val);

	private:
		int GetUniformLoc(const std::string& name);
		std::string LoadShader(const std::string& filePath);
		int CreateShader(const std::string& source, unsigned int type);
		void CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
	private:
		uint32_t m_SProgram;
		bool mutable m_IsProgramBound;
	};
}
