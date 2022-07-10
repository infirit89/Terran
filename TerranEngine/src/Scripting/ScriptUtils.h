#pragma once

extern "C"
{
    typedef struct _MonoException MonoException;
}

namespace TerranEngine
{
    class ScriptUtils
    {
    public:
        static void PrintUnhandledException(MonoException* monoException);
    };
}
