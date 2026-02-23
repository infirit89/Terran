#include "AssetHandle.h"
#include "AssetManager.h"

namespace Terran::Asset {

AssetHandle::~AssetHandle()
{
    m_asset_manager->enqueue_asset_for_deletion(m_id);
}

bool AssetHandle::is_valid() const
{
    return m_asset_manager->is_asset_loaded(m_id);
}

}
