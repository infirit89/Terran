using System;
using System.Runtime.CompilerServices;

namespace Terran
{
    // ---- Scriptable ----
    public class Scriptable : Component
    {
        public Scriptable() 
            : base(default) 
        {
        }

        internal Scriptable(UUID id)
            : base(id) 
        {
        }

        protected virtual void Init() { }
        protected virtual void Update(float deltaTime) { }

        protected virtual void OnCollisionBegin(Entity other) { }
        protected virtual void OnCollisionEnd(Entity other) { }

        protected virtual void PhysicsUpdate() { }
    }
    // --------------------

    // ---- Tag ----
    public class Tag : Component
    {

        internal Tag(UUID id)
            : base(id) { }

        public string Name
        {
            get
            {
                unsafe
                {
                    if (Entity != null)
                        return Internal.Tag_GetNameICall(Entity.Handle)!;

                    return "";
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Tag_SetNameICall(Entity.Handle, value);
                }
            }
        }
    }
    // -------------


    // ---- Transform ----
    public class Transform : Component
    {
        internal Transform(UUID id)
            : base(id) { }

        public Vector3 Position
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetPositionICall(Entity.Handle);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetPositionICall(Entity.Handle, in value);
                }
            }
        }

        public Vector3 Rotation
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetRotationICall(Entity.Handle);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetRotationICall(Entity.Handle, in value);
                }
            }
        }

        public Vector3 Scale
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetScaleICall(Entity.Handle);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetScaleICall(Entity.Handle, in value);
                }
            }
        }

        public bool IsDirty
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_IsDirtyICall(Entity.Handle);
                }
            }
        }

        public Vector3 Forward
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetForwardICall(Entity.Handle);
                }
            }
        }
        public Vector3 Up
        {
            get 
            {
                unsafe 
                {
                    return Internal.Transform_GetUpICall(Entity.Handle);
                }
            }
        }
        public Vector3 Right
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetRightICall(Entity.Handle);
                }
            }
        }
    }
    // -------------------
}
