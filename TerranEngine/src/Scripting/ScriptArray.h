#pragma once

#include "ScriptCache.h"

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
        ScriptArray(MonoArray* monoArray);
        ScriptArray(const ScriptArray& other) = default;

        char* GetElementAddress(uint32_t index, int dataSize) const;

        uint32_t Length() const { return m_Length; }

        void Resize(uint32_t size);
        
        template<typename  T>
        static ScriptArray Create(uint32_t size)
        {
            MonoClass* arrayClass = GetMonoClassFromType<T>();
            return ScriptArray(arrayClass, size);
        }
        
        template<typename T>
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
        template<typename  T>
        static MonoClass* GetMonoClassFromType()
        {
            TR_REGISTER_ARRAY_TYPE(uint8_t, "System.Byte");
            TR_REGISTER_ARRAY_TYPE(uint16_t, "System.UInt16");
            TR_REGISTER_ARRAY_TYPE(uint32_t, "System.UInt32");
            TR_REGISTER_ARRAY_TYPE(uint64_t, "System.UInt64");
            
            TR_REGISTER_ARRAY_TYPE(int8_t, "System.SByte");
            TR_REGISTER_ARRAY_TYPE(int16_t, "System.Int16");
            TR_REGISTER_ARRAY_TYPE(int32_t, "System.Int32");
            TR_REGISTER_ARRAY_TYPE(int64_t, "System.Int64");

            TR_REGISTER_ARRAY_TYPE(bool, "System.Boolean");

            TR_REGISTER_ARRAY_TYPE(float, "System.Single");
            TR_REGISTER_ARRAY_TYPE(double, "System.Double");

            TR_REGISTER_ARRAY_TYPE(UUID, "Terran.UUID");
            
            return nullptr;
        }
        
        MonoArray* m_MonoArray = nullptr;
        uint32_t m_Length = 0;
        MonoClass* m_ElementClass = nullptr;
    };
}
