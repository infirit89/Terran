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


    std::vector<const char*> PhysicsLayerManager::GetLayerNames()
    {
        // TODO: maybe cache?
        std::vector<const char*> layerNames;

        for (int i = 0; i < TR_MAX_LAYER_COUNT; i++)
        {
            PhysicsLayer& layer = GetLayer(i);
            if (!layer.Name.empty())
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
            m_Layers[layer1].Mask &= !(1 << layer2);
            m_Layers[layer2].Mask &= !(1 << layer1);
        }
    }
}
