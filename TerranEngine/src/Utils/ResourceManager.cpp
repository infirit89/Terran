#include "trpch.h"

#include "ResourceManager.h"

namespace TerranEngine 
{
    Shared<Shader> ResourceManager::LoadShader(const char* name)
    {
        return Shared<Shader>();
    }

    Shared<Shader> ResourceManager::LoadShader(const char* fName, const char* vName )
    {
        return shader;
    }

    Shared<Texture> ResourceManager::LoadTexture(const char* name)
    {
        return Shared<Texture>();
    }

    Shared<Shader> ResourceManager::GetShader(const char* name)
    {
        return Shared<Shader>();
    }

    Shared<Texture> ResourceManager::GetTexture(const char* name)
    {
        return Shared<Texture>();
    }
}

