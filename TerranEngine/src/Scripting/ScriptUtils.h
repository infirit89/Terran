#pragma once

#include "GCManager.h"

extern "C"
{
    typedef struct _MonoException MonoException;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoObject MonoObject;
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
    };
}
