#pragma once

#include <string>
#include <glm/mat4x4.hpp>

namespace TerranEngine 
{
	class Shader 
	{
	public:
		Shader();
		Shader(const char* vertexPath, const char* fragmentPath);
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

	private:
		int GetUniformLoc(const char* name);
		int CreateShader(const char* source, unsigned int type);
		void CreateProgram(const char* vertexSource, const char* fragmentSource);

	private:
		uint32_t m_SProgram;
		bool mutable m_IsProgramBound;
	};
}
