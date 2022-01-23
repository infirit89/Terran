using System;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Component 
    {
        public Entity entity;
    }

    public class Transform : Component
    {
        public Vector3 Position 
        {
            get
            {
                Vector3 pos;
                GetTransformPosition_Internal(out pos);

                return pos;
            }

            set 
            {
                SetTransformPosition_Internal(value);
            }
        }

        Vector3 Rotation;

        Vector3 Scale;

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformPosition_Internal(out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformPosition_Internal(in Vector3 position);
    }
}
