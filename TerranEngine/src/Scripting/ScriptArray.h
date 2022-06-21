#pragma once

#include "ScriptCache.h"

#include <type_traits>

extern "C"
{
    typedef struct _MonoArray MonoArray;
    typedef struct _MonoClass MonoClass;
}

namespace TerranEngine
{
    class ScriptArray
    {
    public:
#define TR_REGISTER_ARRAY_TYPE(type, klass)\
    if(std::is_same<T, type>()) return ScriptCache::GetCachedClassFromName(klass)->GetMonoClass()
        
        ScriptArray() = default;
        ~ScriptArray() = default;
        ScriptArray(MonoClass* arrayClass, uint32_t size);
        ScriptArray(const ScriptArray& other);

        char* GetElementAddress(uint32_t index, int dataSize) const;

        uintptr_t Length() const;
        
        template<typename  T>
        static MonoClass* GetMonoClassFromType()
        {
            TR_REGISTER_ARRAY_TYPE(uint8_t, "System.Byte");
            TR_REGISTER_ARRAY_TYPE(int16_t, "System.Short");
            TR_REGISTER_ARRAY_TYPE(int32_t, "System.Int");
            TR_REGISTER_ARRAY_TYPE(int64_t, "System.Long");

            TR_REGISTER_ARRAY_TYPE(bool, "System.Bool");

            TR_REGISTER_ARRAY_TYPE(float, "System.Float");
            TR_REGISTER_ARRAY_TYPE(double, "System.Double");

            return nullptr;
        }
        
        template<typename  T>
        static ScriptArray Create(uint32_t size)
        {
            MonoClass* arrayClass = GetMonoClassFromType<T>();
            return ScriptArray<T>(arrayClass, size);
        }
        
        template<typename  T>
        void Set(uint32_t index, T value)
        {
            T* elementAdrr = (T*)GetElementAddress(index, sizeof(T));
            *elementAdrr = value;  
        }

        template<typename  T>
        T Get(uint32_t index)
        {
            return *(T*)GetElementAddress(index, sizeof(T));
        }
        
        inline MonoArray* GetMonoArray() const { return m_MonoArray; } 
        
    private:
        MonoArray* m_MonoArray = nullptr;
    };
}
