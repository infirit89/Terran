#include "trpch.h"
#include "PhysicsLayerManager.h"

namespace TerranEngine
{
    PhysicsLayerManager::Layers PhysicsLayerManager::m_Layers;

    void PhysicsLayerManager::SetLayerName(int index, const std::string& layerName)
    {
        m_Layers.at(index).Name = layerName;
    }

    std::vector<std::string_view> PhysicsLayerManager::GetLayerNames()
    {
        // TODO: maybe cache?
        std::vector<std::string_view> layerNames;

        for (int i = 0; i < s_MaxLayerCount; i++)
        {
            PhysicsLayer& layer = GetLayer(i);
            layerNames.push_back(layer.Name.c_str());
        }

        return layerNames;
    }

    std::string& PhysicsLayerManager::GetLayerName(int index)
    {
        return m_Layers.at(index).Name;
    }

    bool PhysicsLayerManager::CanLayersCollide(int layer1, int layer2)
    {
        bool canCollide1 = (m_Layers[layer1].Mask & (1 << layer2)) != 0;
        bool canCollide2 = (m_Layers[layer2].Mask & (1 << layer1)) != 0;

        return canCollide1 && canCollide2;
    }

    void PhysicsLayerManager::SetLayerMask(int layer1, int layer2, bool canCollide)
    {
        if (canCollide)
        {
            m_Layers[layer1].Mask |= 1 << layer2;
            m_Layers[layer2].Mask |= 1 << layer1;
        }
        else 
        {
            m_Layers[layer1].Mask &= ~(1 << layer2);
            m_Layers[layer2].Mask &= ~(1 << layer1);
        }
    }
}
