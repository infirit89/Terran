#pragma once
#include <string>
#include <cstdint>
#include <array>

namespace TerranEngine 
{
#define TR_MAX_LAYER_COUNT 16
    struct PhysicsLayer
    {
        uint16_t Mask = 0xFFFF;
        std::string Name;
    };

    class PhysicsLayerManager
    {
        using Layers = std::array<PhysicsLayer, TR_MAX_LAYER_COUNT>;
    public:
        static void AddLayer(int index, const std::string& layerName);
        static void RemoveLayer(int index);

        static Layers& GetLayers() { return m_Layers; }
        static PhysicsLayer& GetLayer(int index) { return m_Layers[index]; }

        static uint16_t GetLayerCategory(int index) { return 1 << index; }

        static std::vector<const char*> GetLayerNames();
        static std::string& GetLayerName(int index);
        
        static bool CanLayersCollide(int layer1, int layer2);
        static void SetLayerMask(int layer1, int layer2, bool canCollide);

    private:
        static Layers m_Layers;
    };
}

