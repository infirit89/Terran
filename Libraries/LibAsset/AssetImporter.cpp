#include "AssetImporter.h"
#include "AssetTypes.h"
#include "AssetLoader.h"
#include "AssetMetadata.h"
#include "Asset.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>

namespace Terran {
namespace Asset {

std::unordered_map<AssetTypeId, Terran::Core::Shared<AssetLoader>> AssetImporter::s_loaders;


void AssetImporter::load(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset>& asset)
{
    const AssetTypeId type_id = assetMetadata.Type.value().type_id;
    if (s_loaders.contains(type_id)) {
        s_loaders[type_id]->Load(assetMetadata, asset);
        return;
    }

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
}

bool AssetImporter::save(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset> const& asset)
{
    const AssetTypeId type_id = assetMetadata.Type.value().type_id;
    if (s_loaders.contains(type_id))
        return s_loaders[type_id]->Save(assetMetadata, asset);

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
    return false;
}

}

}
