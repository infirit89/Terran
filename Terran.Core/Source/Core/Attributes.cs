using System;
using System.Runtime.CompilerServices;

namespace Terran
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializeFieldAttribute : Attribute 
    {
    }

    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    public sealed class SourceInfoAttribute([CallerFilePath] string callerFilePath = "") : Attribute
    {
        internal string CallerFilePath = callerFilePath;
    }
}
