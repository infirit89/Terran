#pragma once

#include "Renderer/Font.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Core/Base.h"

#include <unordered_map>

namespace TerranEngine 
{
	class ResourceManager 
	{
	public:	
		static Shared<Shader> LoadShader(const char* name);
		static Shared<Shader> LoadShader(const char* name, const char* vName, const char* fName);

		static Shared<Texture> LoadTexture(const char* name);

		static Shared<Shader> GetShader(const char* name);
		static Shared<Texture> GetTexture(const char* name);

	private:
		static std::unordered_map<std::string, Shared<Shader>> m_Shaders;
		static std::unordered_map<std::string, Shared<Texture>> m_Textures;
	};
}
