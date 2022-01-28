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
                GetTransformPosition_Internal(entity.RuntimeID, out Vector3 pos);
                return pos;
            }

            set
            {
                SetTransformPosition_Internal(entity.RuntimeID, value);
            }
        }

        Vector3 Rotation
        {
            get
            {
                GetTransformRotation_Internal(entity.RuntimeID, out Vector3 rot);
                return rot;
            }

            set
            {
                SetTransformRotation_Internal(entity.RuntimeID, value);
            }
        }

        Vector3 Scale
        {
            get
            {
                GetTransformScale_Internal(entity.RuntimeID, out Vector3 scale);
                return scale;
            }

            set
            {
                SetTransformScale_Internal(entity.RuntimeID, value);
            }
        }

        // ---- position ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformPosition_Internal(uint entityID, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformPosition_Internal(uint entityID, in Vector3 position);
        // ------------------

        // ---- rotation ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformRotation_Internal(uint entityID, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformRotation_Internal(uint entityID, in Vector3 position);
        // ------------------

        // ---- scale ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformScale_Internal(uint entityID, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformScale_Internal(uint entityID, in Vector3 position);
        // ---------------
    }
}
