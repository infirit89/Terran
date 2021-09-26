#include "trpch.h"
#include "Core/UUID.h"

namespace TerranEngine
{
    UUID::UUID()
        : m_Data({ 0 })
    {
        Generate();
    }

    UUID::UUID(const std::array<uint8_t, 16>& data)
        : m_Data(data)
    {
    }
}
