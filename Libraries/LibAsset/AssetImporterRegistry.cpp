#include "AssetImporterRegistry.h"
#include "Asset.h"
#include "AssetImporter.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>

#include <unordered_map>

namespace Terran::Asset {

std::unordered_map<AssetTypeId, Terran::Core::Shared<AssetImporter>> AssetImporterRegistry::s_loaders;

void AssetImporterRegistry::load(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset>& asset)
{
    AssetTypeId const type_id = assetMetadata.Type;
    if (s_loaders.contains(type_id)) {
        s_loaders[type_id]->load(assetMetadata, asset);
        return;
    }

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
}

bool AssetImporterRegistry::save(AssetMetadata const& assetMetadata, Terran::Core::Shared<Asset> const& asset)
{
    AssetTypeId const type_id = assetMetadata.Type;
    if (s_loaders.contains(type_id))
        return s_loaders[type_id]->save(assetMetadata, asset);

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
    return false;
}

}
