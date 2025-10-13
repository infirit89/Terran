#pragma once

#include "LibCore/Base.h"
#include "LibCore/Buffer.h"

#include <stdint.h>

namespace TerranEngine {

class UniformBuffer final {
public:
    UniformBuffer(uint32_t bufferSize, uint32_t bindingPoint);
    ~UniformBuffer();

    static Terran::Core::Shared<UniformBuffer> Create(uint32_t size, uint32_t bindingPoint);

    void SetData(void const* data, uint32_t offset, uint32_t size);
    // void SetData(const void* newData, uint32_t size);

private:
    void Release();

    Terran::Core::Buffer m_LocalData;
    uint32_t m_Handle;
};

}
