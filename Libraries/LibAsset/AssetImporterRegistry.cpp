#include "AssetImporterRegistry.h"
#include "Asset.h"
#include "AssetImporter.h"
#include "AssetImporterError.h"
#include "AssetMetadata.h"
#include "AssetTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>
#include <LibCore/RefPtr.h>

#include <LibCore/Result.h>
#include <unordered_map>

namespace Terran::Asset {

std::unordered_map<AssetTypeId, Terran::Core::Shared<AssetImporter>> AssetImporterRegistry::s_loaders;

AssetLoadResult AssetImporterRegistry::load(AssetMetadata const& assetMetadata)
{
    AssetTypeId const type_id = assetMetadata.Type;
    if (s_loaders.contains(type_id)) {
        return s_loaders[type_id]->load(assetMetadata);
    }

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
    return { Core::CreateShared<AssetImporterError>(AssetImporterError::Code::ImporterNotFound) };
}

bool AssetImporterRegistry::save(AssetMetadata const& assetMetadata, Core::RefPtr<Asset> const& asset)
{
    if(assetMetadata.Type != asset->type()) {
        return false;
    }

    AssetTypeId const type_id = assetMetadata.Type;
    if (s_loaders.contains(type_id))
        return s_loaders[type_id]->save(assetMetadata, asset);

    TR_CORE_ERROR(TR_LOG_ASSET, "Invalid asset type for asset: {0}", assetMetadata.Path);
    return false;
}

}
