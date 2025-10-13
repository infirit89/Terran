#pragma once

#include "LibCore/UUID.h"

namespace TerranEditor {

class AssetEditorPanel {
public:
    AssetEditorPanel() = default;
    virtual ~AssetEditorPanel() = default;

    virtual void OnRender() = 0;
    void SetOpen(bool open, Terran::Core::UUID const& id)
    {
        m_Open = open;
        m_ID = id;
    }

protected:
    bool m_Open = false;
    Terran::Core::UUID m_ID;
};

}
