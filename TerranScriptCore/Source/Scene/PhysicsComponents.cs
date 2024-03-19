namespace Terran
{
    public class Rigidbody2D : Component
    {
        public Rigidbody2D() { }

        internal Rigidbody2D(Entity entity)
        {
            Entity = entity;
        }

        public bool FixedRotation
        {
            get => Internal.Rigidbody2D_IsFixedRotation(Entity.ID);
            set => Internal.Rigidbody2D_SetFixedRotation(Entity.ID, value);
        }

        public RigidbodySleepState AwakeState
        {
            get => (RigidbodySleepState)Internal.Rigidbody2D_GetSleepState(Entity.ID);
            set => Internal.Rigidbody2D_SetSleepState(Entity.ID, (byte)value);
        }

        public float GravityScale
        {
            get => Internal.Rigidbody2D_GetGravityScale(Entity.ID);
            set => Internal.Rigidbody2D_SetGravityScale(Entity.ID, value);
        }

        public RigidbodyType BodyType
        {
            get => (RigidbodyType)Internal.Rigidbody2D_GetType(Entity.ID);
            set => Internal.Rigidbody2D_SetType(Entity.ID, (byte)value);
        }

        public Vector2 LinearVelocity
        {
            get
            {
                Vector2 linearVelocity;
                Internal.Rigidbody2D_GetLinearVelocity(Entity.ID, out linearVelocity);
                return linearVelocity;
            }

            set => Internal.Rigidbody2D_SetLinearVelocity(Entity.ID, in value);
        }

        public float AngularVelocity
        {
            get => Internal.Rigidbody2D_GetAngularVelocity(Entity.ID);
            set => Internal.Rigidbody2D_SetAngularVelocity(Entity.ID, value);
        }

        public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForce(Entity.ID, in force, in position, (byte)forceMode);

        public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForceAtCenter(Entity.ID, in force, (byte)forceMode);
    }

    public enum ColliderType2D : byte
    {
        None = 0,
        Box,
        Circle,
        Capsule
    }

    public class Collider2D : Component
    {
        public Collider2D() { }

        protected Collider2D(ColliderType2D type)
        {
            p_ColliderType = type;
        }

        public Vector2 Offset
        {
            get
            {
                Vector2 offset;
                Internal.Collider2D_GetOffset(Entity.ID, (byte)p_ColliderType, out offset);
                return offset;
            }
            set => Internal.Collider2D_SetOffset(Entity.ID, (byte)p_ColliderType, in value);
        }

        public bool IsSensor
        {
            get => Internal.Collider2D_IsSensor(Entity.ID, (byte)p_ColliderType);
            set => Internal.Collider2D_SetSensor(Entity.ID, (byte)p_ColliderType, value);
        }

        public ColliderType2D ColliderType => p_ColliderType;

        protected ColliderType2D p_ColliderType = ColliderType2D.None;
    }

    public class BoxCollider2D : Collider2D
    {
        public BoxCollider2D() : base(ColliderType2D.Box) { }

        public Vector2 Size
        {
            get
            {
                Vector2 size;
                Internal.BoxCollider2D_GetSize(Entity.ID, out size);
                return size;
            }

            set => Internal.BoxCollider2D_SetSize(Entity.ID, in value);
        }
    }

    public class CircleCollider2D : Collider2D
    {
        public CircleCollider2D() : base(ColliderType2D.Circle) { }

        public float Radius
        {
            get => Internal.CircleCollider2D_GetRadius(Entity.ID);
            set => Internal.CircleCollider2D_SetRadius(Entity.ID, value);
        }
    }

    public class CapsuleCollider2D : Collider2D
    {
        public CapsuleCollider2D() : base(ColliderType2D.Capsule) { }

        public Vector2 Size
        {
            get
            {
                Vector2 size;
                Internal.CapsuleCollider2D_GetSize(Entity.ID, out size);
                return size;
            }

            set => Internal.CapsuleCollider2D_SetSize(Entity.ID, in value);
        }
    }
}
