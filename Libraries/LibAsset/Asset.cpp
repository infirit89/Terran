#include "Asset.h"
#include "AssetManager.h"

namespace Terran::Asset {

Asset::~Asset()
{
    if (m_asset_manager) {
        m_asset_manager->enqueue_asset_for_deletion(m_handle);
    }
}

}
