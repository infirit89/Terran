using System;
using System.Runtime.CompilerServices;

namespace Terran
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializeFieldAttribute : Attribute 
    {
    }

    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    public sealed class SourceInfoAttribute : Attribute
    {
        public SourceInfoAttribute([CallerFilePath] string callerFilePath = "")
        {
            CallerFilePath = callerFilePath;
        }

        internal string CallerFilePath;
    }
}
