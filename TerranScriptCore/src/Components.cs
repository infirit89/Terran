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
                if (entity != null) 
                {
                    GetTransformPosition_Internal(entity.RuntimeID, out Vector3 pos);
                    return pos;
                }

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformPosition_Internal(entity.RuntimeID, value);
                    return;
                }
                // TODO: log that the entity is null
            }
        }

        public Vector3 Rotation
        {
            get
            {
                if (entity != null) 
                {
                    GetTransformRotation_Internal(entity.RuntimeID, out Vector3 rot);
                    return rot;
                }

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformRotation_Internal(entity.RuntimeID, value);
                    return;
                }

                // TODO: log that the entity is null
            }
        }

        public Vector3 Scale
        {
            get
            {
                if (entity != null) 
                {
                    GetTransformScale_Internal(entity.RuntimeID, out Vector3 scale);
                    return scale;
                }

                // TODO: log that the entity is null
                return new Vector3(0.0f, 0.0f, 0.0f);
            }

            set
            {
                if (entity != null) 
                {
                    SetTransformScale_Internal(entity.RuntimeID, value);
                    return;
                }

                // TODO: log that the entity is null
            }
        }

        // ---- position ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformPosition_Internal(uint entityID, out Vector3 outPosition);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformPosition_Internal(uint entityID, in Vector3 inPosition);
        // ------------------

        // ---- rotation ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformRotation_Internal(uint entityID, out Vector3 outRotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformRotation_Internal(uint entityID, in Vector3 inRotation);
        // ------------------

        // ---- scale ----
        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void GetTransformScale_Internal(uint entityID, out Vector3 outScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void SetTransformScale_Internal(uint entityID, in Vector3 inScale);
        // ---------------
    }
}
