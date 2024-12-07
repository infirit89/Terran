#pragma once
#include <string>
#include <cstdint>
#include <array>
#include <vector>

namespace TerranEngine 
{
    struct PhysicsLayer final
    {
        uint16_t Mask = 0xFFFF;
        std::string Name;
    };

    class PhysicsLayerManager final
    {
        static constexpr size_t s_MaxLayerCount = 16;
        using Layers = std::array<PhysicsLayer, s_MaxLayerCount>;
    public:
        static void SetLayerName(int index, const std::string& layerName);

        static Layers& GetLayers() { return m_Layers; }
        static PhysicsLayer& GetLayer(int index) { return m_Layers[index]; }

        static uint16_t GetLayerCategory(int index) { return 1 << index; }

        static std::vector<std::string_view> GetLayerNames();
        static std::string& GetLayerName(int index);
        
        static bool CanLayersCollide(int layer1, int layer2);
        static void SetLayerMask(int layer1, int layer2, bool canCollide);

        static constexpr size_t GetMaxLayerCount() { return s_MaxLayerCount; }

    private:
        static Layers m_Layers;
    };
}

