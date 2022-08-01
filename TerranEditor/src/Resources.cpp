#include "Resources.h"

namespace TerranEditor 
{
    using namespace TerranEngine;
    struct ResourceData
    {
        Shared<Texture> DirectoryTexture;
        Shared<Texture> FileTexture;
    };
    
    static ResourceData s_ResData;
    void Resources::Init()
    {
        s_ResData.FileTexture = CreateShared<Texture>("Resources/Textures/temp_file_icon.png");
        s_ResData.DirectoryTexture = CreateShared<Texture>("Resources/Textures/temp_folder_icon.png");
    }

    void Resources::Shutdown() 
    {
        s_ResData.DirectoryTexture.reset();
        s_ResData.FileTexture.reset();
    }

    TerranEngine::Shared<TerranEngine::Texture> Resources::GetDirectoryTexture() { return s_ResData.DirectoryTexture; }

    TerranEngine::Shared<TerranEngine::Texture> GetFileTexture() { return s_ResData.FileTexture; }
}

