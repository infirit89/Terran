#pragma once

#include "ScriptType.h"

#include "Utils/Variant.h"

extern "C"
{
    typedef struct _MonoArray MonoArray;
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
}

namespace TerranEngine
{
    namespace detail
    {
        template<typename  T>
        MonoClass* ScriptArrayCreate(uint32_t size)
        {
            // MonoClass* arrayClass = GetMonoClassFromType<T>();
            //return MonoClass*(arrayClass, size);
            //static_assert(false);
            return nullptr;
        }

        template<>
        MonoClass* ScriptArrayCreate<uint8_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<uint16_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<uint32_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<uint64_t>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<int8_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<int16_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<int32_t>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<int64_t>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<bool>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<float>(uint32_t size);
        template<>
        MonoClass* ScriptArrayCreate<double>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<std::string>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<MonoObject*>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<glm::vec2>(uint32_t size);

        template<>
        MonoClass* ScriptArrayCreate<UUID>(uint32_t size);
    }

    class ScriptArray
    {
    public:
        ScriptArray() = default;
        ~ScriptArray() = default;
        ScriptArray(MonoClass* arrayClass, uint32_t size);
        ScriptArray(const ScriptArray& other) = default;

        char* GetElementAddress(uint32_t index, int dataSize) const;

        Utils::Variant At(uint32_t index) const;
        Utils::Variant operator[](uint32_t index) const { return At(index); }
        
        size_t Length() const { return m_Length; }

        void Resize(size_t size);
        
        template<typename T>
        void Set(uint32_t index, T value)
        {
            if constexpr (std::is_same<T, Utils::Variant>::value)
                SetData(value, index);
            else
            {
                T* elementAdrr = (T*)GetElementAddress(index, sizeof(T));
                *elementAdrr = value;
            }
        }

        template<typename  T>
        T Get(uint32_t index)
        {
            return *(T*)GetElementAddress(index, sizeof(T));
        }
        
        inline MonoArray* GetMonoArray() const { return m_MonoArray; } 
        inline const ScriptType& GetType() const { return m_Type; }
        inline operator bool() const { return m_MonoArray != nullptr; }

    public:
        template<typename T>
        static ScriptArray Create(uint32_t size)
        {
            MonoClass* arrayClass = detail::ScriptArrayCreate<T>(size);
            return ScriptArray(arrayClass, size);
        }

        static ScriptArray Create(MonoArray* monoArray);
    
    private:
        void SetData(const Utils::Variant& value, uint32_t index);
        
        // template<typename  T>
        // static MonoClass* GetMonoClassFromType();
        
        MonoArray* m_MonoArray = nullptr;
        size_t m_Length = 0;
        ScriptType m_Type = {};        
    };
}
