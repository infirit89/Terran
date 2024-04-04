using Microsoft.VisualBasic;

namespace Terran
{
    public class Rigidbody2D : Component
    {
        internal Rigidbody2D(UUID id)
            : base(id) { }

        public bool FixedRotation
        {
            get
            {
                unsafe
                {
                    return Internal.Rigidbody2D_IsFixedRotationICall(Entity.ID);
                }
            }
            set
            {
                unsafe
                {
                    Internal.Rigidbody2D_SetFixedRotationICall(Entity.ID, value);
                }
            }
        }

        public RigidbodySleepState SleepState
        {
            get 
            {
                unsafe 
                {
                    return Internal.Rigidbody2D_GetSleepStateICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Rigidbody2D_SetSleepStateICall(Entity.ID, value);
                }
            }
        }

        public float GravityScale
        {
            get
            {
                unsafe
                {
                    return Internal.Rigidbody2D_GetGravityScaleICall(Entity.ID);
                }
            } 
            set
            {
                unsafe 
                {
                    Internal.Rigidbody2D_SetGravityScaleICall(Entity.ID, value);
                }
            }
        }

        public RigidbodyType BodyType
        {
            get 
            {
                unsafe
                {
                    return Internal.Rigidbody2D_GetTypeICall(Entity.ID);
                }
            }
            set
            {
                unsafe
                {
                    Internal.Rigidbody2D_SetTypeICall(Entity.ID, value);
                }
            }
        }

        public Vector2 LinearVelocity
        {
            get
            {
                unsafe
                {
                    return Internal.Rigidbody2D_GetLinearVelocityICall(Entity.ID);
                }
            }

            set
            {
                unsafe 
                {
                    Internal.Rigidbody2D_SetLinearVelocityICall(Entity.ID, in value);
                }
            }
        }

        public float AngularVelocity
        {
            get
            {
                unsafe 
                {
                    return Internal.Rigidbody2D_GetAngularVelocityICall(Entity.ID);
                }
            }
            set
            {
                unsafe
                {
                    Internal.Rigidbody2D_SetAngularVelocityICall(Entity.ID, value);
                }
            }
        }

        public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode)
        {
            unsafe
            {
                Internal.Rigidbody2D_ApplyForceICall(Entity.ID, in force, in position, forceMode);
            }
        }

        public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode)
        {
            unsafe
            {
                Internal.Rigidbody2D_ApplyForceAtCenterICall(Entity.ID, in force, forceMode);
            }
        }
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
        internal Collider2D(UUID id) 
            : base(id) { }

        protected Collider2D(UUID id, ColliderType2D type)
            : base(id)
        {
            m_ColliderType = type;
        }

        public Vector2 Offset
        {
            get
            {
                unsafe 
                {
                    Internal.Collider2D_GetOffsetICall(Entity.ID, m_ColliderType, out var offset);
                    return offset;
                }
            }
            set
            {
                unsafe
                {
                    Internal.Collider2D_SetOffsetICall(Entity.ID, m_ColliderType, in value);
                }
            }
        }

        public bool IsSensor
        {
            get
            {
                unsafe
                {
                    Internal.Collider2D_IsSensorICall(Entity.ID, m_ColliderType, out var sensor);
                    return sensor;
                }
            }
            set
            {
                unsafe 
                {
                    Internal.Collider2D_SetSensorICall(Entity.ID, m_ColliderType, value);
                }
            }
        }

        public ColliderType2D ColliderType => m_ColliderType;

        protected ColliderType2D m_ColliderType = ColliderType2D.None;
    }

    public class BoxCollider2D : Collider2D
    {
        internal BoxCollider2D(UUID id) : base(id, ColliderType2D.Box) { }

        public Vector2 Size
        {
            get
            {
                unsafe 
                {
                    Internal.BoxCollider2D_GetSizeICall(Entity.ID, out var size);
                    return size;
                }
            }

            set 
            {
                unsafe
                {
                    Internal.BoxCollider2D_SetSizeICall(Entity.ID, in value);
                }
            }
        }
    }

    public class CircleCollider2D : Collider2D
    {
        internal CircleCollider2D(UUID id) : base(id, ColliderType2D.Circle) { }

        public float Radius
        {
            get
            {
                unsafe
                {
                    return Internal.CircleCollider2D_GetRadiusICall(Entity.ID);
                }
            }
            set
            {
                unsafe 
                {
                    Internal.CircleCollider2D_SetRadiusICall(Entity.ID, value);
                }
            }
        }
    }

    public class CapsuleCollider2D : Collider2D
    {
        internal CapsuleCollider2D(UUID id) : base(id, ColliderType2D.Capsule) { }

        public Vector2 Size
        {
            get
            {
                unsafe 
                {
                    Internal.CapsuleCollider2D_GetSizeICall(Entity.ID, out var size);
                    return size;
                }
            }

            set
            {
                unsafe 
                {
                    Internal.CapsuleCollider2D_SetSizeICall(Entity.ID, in value);
                }
            }
        }
    }
}
