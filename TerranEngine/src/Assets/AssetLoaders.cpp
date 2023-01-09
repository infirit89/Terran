#include "trpch.h"
#include "AssetLoaders.h"

#include "AssetManager.h"

#include "Graphics/Texture.h"

namespace TerranEngine 
{
    void TextureAssetLoader::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        asset = CreateShared<Texture>(AssetManager::GetFileSystemPath(assetInfo.Path));
    }

    bool TextureAssetLoader::Save(const Shared<Asset>& asset)
    {
        return false;
    }


    void TextAssetLoader::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        std::ifstream in(AssetManager::GetFileSystemPath(assetInfo.Path), std::ios::in);
        std::string result;
        if (in)
        {
            in.seekg(0, in.end);
            size_t size = in.tellg();

            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, in.beg);
                in.read(&result[0], size);
            }
            else
                TR_ERROR("Couldn't read the file");
        }
        else
            TR_ERROR("Couldn't open the file");

        asset = CreateShared<TextAsset>(result);
    }

    bool TextAssetLoader::Save(const Shared<Asset>& asset)
    {
        return false;
    }

}