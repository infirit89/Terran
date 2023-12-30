#pragma once

extern "C"
{
    typedef struct _MonoType MonoType;
    typedef struct _MonoClass MonoClass;
}

namespace TerranEngine
{
    enum class NativeType
    {
        None = 0,
        Bool,
        Char,
        Int8,
        Int16,
        Int32,
        Int64,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Float,
        Double,
        String,
        Vector2,
        Vector3,
        Color,
        UUID,
        Entity,
        LayerMask
    };

    class ManagedClass;
    class ManagedType
    {
    public:
        ManagedType() = default;
        ManagedType(MonoType* monoType);
        ManagedType(MonoClass* monoClass);
        
        ManagedType(const ManagedType& other);
        ~ManagedType() = default;

        inline MonoType* GetMonoType() const { return m_MonoType; }
        inline MonoClass* GetTypeClass() const { return m_TypeClass; }
        NativeType GetNativeType() const { return m_NativeType; }
        inline int GetSize() const { return m_Size; }
        
        bool IsArray() const;
        bool IsPointer() const;
        bool IsVoid() const;
        bool IsObject() const;
        bool IsEnum() const;

        ManagedType& operator=(const ManagedType& other); 
        
    private:
        MonoType* m_MonoType = nullptr;
        MonoClass* m_TypeClass = nullptr;
        NativeType m_NativeType = NativeType::None;
        int m_TypeEncoding = 0;
        int m_Size = 0;
    };
}
