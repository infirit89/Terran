#include "trpch.h"
#include "AssetLoaders.h"

#include "AssetManager.h"

#include "Graphics/Texture.h"

#include "Scene/SceneSerializer.h"

#include <yaml-cpp/yaml.h>
#include <stb_image.h>

namespace TerranEngine 
{
    void TextureAssetLoader::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        asset = CreateTextureFromFile(AssetManager::GetFileSystemPath(assetInfo.Path));
    }

    bool TextureAssetLoader::Save(const AssetInfo& assetInfo, const Shared<Asset>& asset)
    {
        return false;
    }

    Shared<Texture2D> TextureAssetLoader::CreateTextureFromFile(const std::filesystem::path& textureSourcePath)
    {
        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        constexpr int desiredChannels = 0;
        uint8_t* data = stbi_load(textureSourcePath.string().c_str(),
                                &width, &height, &channels, desiredChannels);

        TextureParameters textureParameters;
        switch (channels) 
        {
        case 4:
        {
            textureParameters.Format = TextureFormat::RGBA;
            break;
        }
        case 3: 
        {
            textureParameters.Format = TextureFormat::RGB;
            break;
        }
        }
        textureParameters.Width = width;
        textureParameters.Height = height;
        Shared<Texture2D> texture = CreateShared<Texture2D>(textureParameters, data);
        stbi_image_free(data);

        return texture;
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

    bool TextAssetLoader::Save(const AssetInfo& assetInfo, const Shared<Asset>& asset)
    {
        return false;
    }

    void SceneAssetLoader::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        
    }

    bool SceneAssetLoader::Save(const AssetInfo& assetInfo, const Shared<Asset>& asset)
    {
        SceneSerializer serializer(DynamicCast<Scene>(asset));
        serializer.SerializeEditor(AssetManager::GetFileSystemPath(assetInfo.Path));
        return true;
    }

    void PhysicsMaterial2DAssetLoader::Load(const AssetInfo& assetInfo, Shared<Asset>& asset)
    {
        YAML::Node node;

        try
        {
            node = YAML::LoadFile(AssetManager::GetFileSystemPath(assetInfo.Path).string());
        }
        catch (const YAML::Exception& e)
        {
            TR_ERROR(e.what());
            return;
        }

        try
        {
            auto physicsMaterial = node["PhysicsMaterial2D"];
            Shared<PhysicsMaterial2DAsset> physicsMaterialAsset = CreateShared<PhysicsMaterial2DAsset>();
            physicsMaterialAsset->Density = physicsMaterial["Density"].as<float>();
            physicsMaterialAsset->Friction = physicsMaterial["Friction"].as<float>();
            physicsMaterialAsset->Restitution = physicsMaterial["Restitution"].as<float>();

            asset = physicsMaterialAsset;
        }
        catch (const YAML::BadSubscript& e)
        {
            TR_ERROR(e.what());
            return;
        }
    }

    bool PhysicsMaterial2DAssetLoader::Save(const AssetInfo& assetInfo, const Shared<Asset>& asset)
    {
        YAML::Emitter out;

        Shared<PhysicsMaterial2DAsset> physicsMaterial = DynamicCast<PhysicsMaterial2DAsset>(asset);
        out << YAML::BeginMap;
        out << YAML::Key << "PhysicsMaterial2D" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Density" << YAML::Value << physicsMaterial->Density;
        out << YAML::Key << "Friction" << YAML::Value << physicsMaterial->Friction;
        out << YAML::Key << "Restitution" << YAML::Value << physicsMaterial->Restitution;

        out << YAML::EndMap;
        out << YAML::EndMap;

        std::ofstream ofs(AssetManager::GetFileSystemPath(assetInfo.Path));
        ofs << out.c_str();

        return true;
    }
}