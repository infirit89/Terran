#include "trpch.h"

#include "ResourceManager.h"

namespace TerranEngine 
{
//    Shared<Shader> ResourceManager::LoadShader(const char* name)
//    {
//        Shared<Shader> shader = CreateShared<Shader>(name);
//        m_Shaders[name] = shader;
//        return shader;
//    }

//    Shared<Shader> ResourceManager::LoadShader(const char* name, const char* vName, const char* fName)
//    {
//        Shared<Shader> shader = CreateShared<Shader>(name, vName, fName);
//        m_Shaders[name] = shader;
//        return shader;
//    }

    Shared<Texture> ResourceManager::LoadTexture(const char* name)
    {
        Shared<Texture> texture = CreateShared<Texture>(name);
        m_Textures[name] = texture;
        return texture;
    }

//    Shared<Shader> ResourceManager::GetShader(const char* name)
//    {
//        TR_ASSERT(!(m_Shaders.find(name) == m_Shaders.end()), "Shader not found");

//        return m_Shaders[name];
//    }

//    Shared<Texture> ResourceManager::GetTexture(const char* name)
//    {
//        TR_ASSERT(!(m_Textures.find(name) == m_Textures.end()), "Texture not found");
//
//        return m_Textures[name];
//    }
}

