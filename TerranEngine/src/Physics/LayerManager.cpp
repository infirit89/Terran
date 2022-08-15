#include "trpch.h"
#include "LayerManager.h"

namespace TerranEngine
{
    PhysicsLayerManager::Layers PhysicsLayerManager::m_Layers;

    void PhysicsLayerManager::AddLayer(int index, const std::string& layerName)
    {
        m_Layers[index] = { 0xFFFF, layerName };
    }
    
    void PhysicsLayerManager::RemoveLayer(int index)
    {
        m_Layers[index].Name = "";
    }
}

