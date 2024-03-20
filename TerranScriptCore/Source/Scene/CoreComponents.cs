namespace Terran
{
    // ---- Scriptable ----
    public class Scriptable : Component
    {
        public Scriptable() { }

        internal Scriptable(UUID id)
        {
            if (Entity == null)
                Entity = new Entity(id);
        }

        protected virtual void Init() { }
        protected virtual void Update(float deltaTime) { }

        protected virtual void OnCollisionBegin(Entity entity) { }
        protected virtual void OnCollisionEnd(Entity entity) { }

        protected virtual void PhysicsUpdate() { }
    }
    // --------------------

    // ---- Tag ----
    public class Tag : Component
    {
        public string Name
        {
            get
            {
                unsafe
                {
                    if (Entity != null)
                        return Internal.Tag_GetNameICall(Entity.ID)!;

                    return "";
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Tag_SetNameICall(Entity.ID, value);
                }
            }
        }
    }
    // -------------


    // ---- Transform ----
    public class Transform : Component
    {
        public Vector3 Position
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetPositionICall(Entity.ID);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetPositionICall(Entity.ID, in value);
                }
            }
        }

        public Vector3 Rotation
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetRotationICall(Entity.ID);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetRotationICall(Entity.ID, in value);
                }
            }
        }

        public Vector3 Scale
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetScaleICall(Entity.ID);
                }
            }

            set
            {
                unsafe
                {
                    if (Entity != null)
                        Internal.Transform_SetScaleICall(Entity.ID, in value);
                }
            }
        }

        public bool IsDirty
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_IsDirtyICall(Entity.ID);
                }
            }
        }

        public Vector3 Forward
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetForwardICall(Entity.ID);
                }
            }
        }
        public Vector3 Up
        {
            get 
            {
                unsafe 
                {
                    return Internal.Transform_GetUpICall(Entity.ID);
                }
            }
        }
        public Vector3 Right
        {
            get
            {
                unsafe
                {
                    return Internal.Transform_GetRightICall(Entity.ID);
                }
            }
        }
    }
    // -------------------
}
