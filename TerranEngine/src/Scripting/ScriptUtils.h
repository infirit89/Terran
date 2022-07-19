#pragma once

#include "Core/UUID.h"

#include "GCManager.h"

extern "C"
{
    typedef struct _MonoException MonoException;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoClass MonoClass;
}

namespace TerranEngine
{
    class ScriptUtils
    {
    public:
        static void PrintUnhandledException(MonoException* monoException);

        static void SetFieldDataRaw(void* value, MonoClassField* monoField, GCHandle handle);
        static void GetFieldDataRaw(void* result, MonoClassField* monoField, GCHandle handle);
        static MonoObject* GetFieldValueObject(MonoClassField* monoField, GCHandle handle);

        static void SetFieldDataString(const char* value, GCHandle handle, MonoClassField* monoField);
        static std::string GetFieldDataString(GCHandle handle, MonoClassField* monoField);

        static void SetFieldDataUUID(UUID value, GCHandle handle, MonoClassField* monoField);
        static UUID GetFieldDataUUID(GCHandle handle, MonoClassField* monoField);
    };
}
