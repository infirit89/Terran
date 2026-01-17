
#include "TextAssetImporter.h"
#include "Asset.h"
#include "AssetManager.h"

#include <LibAsset/AssetMetadata.h>
#include <LibCore/Assert.h>
#include <LibCore/Base.h>
#include <LibCore/Log.h>
// #include "Graphics/Texture.h"

// #include "Scene/SceneSerializer.h"

// #include "Physics/PhysicsMaterial.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <yaml-cpp/yaml.h>

namespace Terran::Asset {

// void TextureAssetLoader::Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset)
// {
//     asset = CreateTextureFromFile(AssetManager::GetFileSystemPath(assetInfo.Path));
// }
//
// bool TextureAssetLoader::Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset)
// {
//     return false;
// }
//
// Terran::Core::Shared<Texture2D> TextureAssetLoader::CreateTextureFromFile(std::filesystem::path const& textureSourcePath)
// {
//     stbi_set_flip_vertically_on_load(1);
//
//     int width, height, channels;
//     constexpr int desiredChannels = 0;
//     uint8_t* pixels = stbi_load(textureSourcePath.string().c_str(),
//         &width, &height, &channels, desiredChannels);
//
//     TextureParameters textureParameters;
//     switch (channels) {
//     case 4: {
//         textureParameters.Format = TextureFormat::RGBA;
//         break;
//     }
//     case 3: {
//         textureParameters.Format = TextureFormat::RGB;
//         break;
//     }
//     case 2: {
//         textureParameters.Format = TextureFormat::Grayscale;
//         break;
//     }
//     default:
//         TR_ASSERT(false, "Texture format is not supported");
//         break;
//     }
//     textureParameters.Width = width;
//     textureParameters.Height = height;
//     Terran::Core::Buffer data = Terran::Core::Buffer(pixels, width * height * channels);
//     Terran::Core::Shared<Texture2D> texture = Texture2D::Create(textureParameters, data);
//     stbi_image_free(pixels);
//
//     return texture;
// }

// void TextAssetImporter::load(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset>& asset)
// {
//     std::ifstream in(AssetManager::GetFileSystemPath(assetInfo.Path), std::ios::in);
//     std::string result;
//     if (in) {
//         in.seekg(0, in.end);
//         size_t size = in.tellg();
//
//         if (size != -1) {
//             result.resize(size);
//             in.seekg(0, in.beg);
//             in.read(&result[0], size);
//         } else
//             TR_CORE_ERROR(TR_LOG_CORE, "Couldn't read the file");
//     } else
//         TR_CORE_ERROR(TR_LOG_CORE, "Couldn't open the file");
//
//     asset = Terran::Core::CreateShared<TextAsset>(result);
// }
//
// bool TextAssetImporter::save(AssetMetadata const& assetInfo, Terran::Core::Shared<Asset> const& asset)
// {
//     return false;
// }

// void SceneAssetLoader::Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset)
// {
//     // NOTE: setting the scene handle here because otherwise it
//     // generates another random handle which conflicts with the script engine
//     Terran::Core::Shared<Scene> scene = Terran::Core::CreateShared<Scene>(assetInfo.Handle);
//     SceneSerializer serializer(scene);
//     if (serializer.DeserializeEditor(AssetManager::GetFileSystemPath(assetInfo.Path)) != Result::OK)
//         return;
//
//     asset = scene;
// }
//
// bool SceneAssetLoader::Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset)
// {
//     SceneSerializer serializer(Terran::Core::DynamicCast<Scene>(asset));
//     serializer.SerializeEditor(AssetManager::GetFileSystemPath(assetInfo.Path));
//     return true;
// }

// void PhysicsMaterial2DAssetLoader::Load(AssetInfo const& assetInfo, Terran::Core::Shared<Asset>& asset)
// {
//     YAML::Node node;
//
//     try {
//         node = YAML::LoadFile(AssetManager::GetFileSystemPath(assetInfo.Path).string());
//     } catch (YAML::Exception const& e) {
//         TR_CORE_ERROR(TR_LOG_ASSET, e.what());
//         return;
//     }
//
//     try {
//         auto physicsMaterial = node["PhysicsMaterial2D"];
//         Terran::Core::Shared<PhysicsMaterial2D> physicsMaterialAsset = Terran::Core::CreateShared<PhysicsMaterial2D>();
//         physicsMaterialAsset->Density = physicsMaterial["Density"].as<float>();
//         physicsMaterialAsset->Friction = physicsMaterial["Friction"].as<float>();
//         physicsMaterialAsset->Restitution = physicsMaterial["Restitution"].as<float>();
//
//         asset = physicsMaterialAsset;
//     } catch (YAML::BadSubscript const& e) {
//         TR_CORE_ERROR(TR_LOG_ASSET, e.what());
//     }
// }
//
// bool PhysicsMaterial2DAssetLoader::Save(AssetInfo const& assetInfo, Terran::Core::Shared<Asset> const& asset)
// {
//     YAML::Emitter out;
//
//     Terran::Core::Shared<PhysicsMaterial2D> physicsMaterial = Terran::Core::DynamicCast<PhysicsMaterial2D>(asset);
//     out << YAML::BeginMap;
//     out << YAML::Key << "PhysicsMaterial2D" << YAML::Value << YAML::BeginMap;
//     out << YAML::Key << "Density" << YAML::Value << physicsMaterial->Density;
//     out << YAML::Key << "Friction" << YAML::Value << physicsMaterial->Friction;
//     out << YAML::Key << "Restitution" << YAML::Value << physicsMaterial->Restitution;
//
//     out << YAML::EndMap;
//     out << YAML::EndMap;
//
//     std::ofstream ofs(AssetManager::GetFileSystemPath(assetInfo.Path));
//     ofs << out.c_str();
//
//     return true;
// }

}
