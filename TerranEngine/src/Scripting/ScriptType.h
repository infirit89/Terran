#pragma once

extern "C"
{
    typedef struct _MonoType MonoType;
}

namespace TerranEngine
{
    class ScriptClass;
    class ScriptType
    {
    public:
        ScriptType() = default;
        ScriptType(MonoType* monoType);
        
        ScriptType(const ScriptType& other);
        ~ScriptType();

        static ScriptType FromClass(const ScriptClass& klass);
        ScriptType GetElementType() const;
        
        inline MonoType* GetMonoType() const { return m_MonoType; }
        inline ScriptClass* GetTypeClass() const { return m_TypeClass; }
        inline int GetSize() const { return m_Size; }
        
        bool IsArray() const;
        bool IsPointer() const;
        bool IsVoid() const;
        bool IsObject() const;
        
        ScriptType& operator=(const ScriptType& other); 
        
        enum
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
            Entity
        } TypeEnum = None;
        
    private:
        void GetUnmanagedType(MonoType* monoType);
        
        MonoType* m_MonoType = nullptr;
        ScriptClass* m_TypeClass = nullptr;
        int m_TypeEncoding = 0;
        int m_Size = 0;
    };
    
}
