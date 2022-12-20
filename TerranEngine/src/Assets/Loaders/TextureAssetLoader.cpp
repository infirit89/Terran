#include "trpch.h"
#include "TextureAssetLoader.h"

#include "Graphics/Texture.h"

#include <stb_image.h>

namespace TerranEngine 
{
    Shared<Asset> TextureAssetLoader::Load(const AssetInfo& assetInfo)
    {
        stbi_set_flip_vertically_on_load(1);

        int width = 0, height = 0, channels = 0;
        constexpr int desiredChannels = 0;

        std::string assetPathStr = assetInfo.Path.string();
        stbi_uc* data = stbi_load(assetPathStr.c_str(), &width, &height, &channels, desiredChannels);

        if (!data)
        {
            TR_ERROR("Couldn't load the texture with path: {0}", assetInfo.Path);
            return nullptr;
        }

        TextureParameters textureParamters;

        switch (channels)
        {
        case 4: textureParamters.TextureType = TextureType::RGBA; break;
        case 3: textureParamters.TextureType = TextureType::RGB; break;
        }

        Shared<Texture> texture = CreateShared<Texture>(width, height, textureParamters);
        texture->SetData(data);
        texture->m_Type = assetInfo.Type;

        stbi_image_free(data);

        return texture;
    }

    bool TextureAssetLoader::Save(const Shared<Asset>& asset)
    {
        return false;
    }
}
