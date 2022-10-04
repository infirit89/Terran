
namespace Terran
{
	public class Component
	{
		public Entity Entity { internal set; get; }
	}

	public class CircleRenderer : Component
	{
		public Color Color
		{
			get => Internal.CircleRenderer_GetColor(Entity.ID);
			set => Internal.CircleRenderer_SetColor(Entity.ID, value);
		}

		public float Thickness
		{
			get => Internal.CircleRenderer_GetThickness(Entity.ID);
			set => Internal.CircleRenderer_SetThickness(Entity.ID, value);
		}
	}

	public class Camera : Component
	{
		public bool IsPrimary
		{
			get => Internal.Camera_IsPrimary(Entity.ID);
			set => Internal.Camera_SetPrimary(Entity.ID, value);
		}

		public Color BackgroundColor
		{
			get => Internal.Camera_GetBackgroundColor(Entity.ID);
			set => Internal.Camera_SetBackgroundColor(Entity.ID, value);
		}
	}
	
	public class SpriteRenderer : Component
	{
		public Color Color
		{
			get => Internal.SpriteRenderer_GetColor(Entity.ID);
			set => Internal.SpriteRenderer_SetColor(Entity.ID, value);
		}
	}

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
		public CapsuleCollider2D() : base(ColliderType2D.Circle) { }

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

	// ---- Scriptable ----
	public class Scriptable : Component
	{
		public Scriptable() { }

		internal Scriptable(byte[] id)
		{
			if (Entity == null) 
				Entity = new Entity(id);
		}

		protected virtual void Init() { }
		protected virtual void Update() { }

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
				if (Entity != null) 
					return Internal.Tag_GetName(Entity.ID);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(Entity != null)
					Internal.Tag_SetName(Entity.ID, value);

				// TODO: log that the entity is null
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
				if (Entity != null) 
				{
					Vector3 outVec =  Internal.Transform_GetPosition(Entity.ID);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (Entity != null) 
					Internal.Transform_SetPosition(Entity.ID, in value);
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (Entity != null) 
					return Internal.Transform_GetRotation(Entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (Entity != null) 
					Internal.Transform_SetRotation(Entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public Vector3 Scale
		{
			get
			{
				if (Entity != null) 
					return Internal.Transform_GetScale(Entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (Entity != null) 
					Internal.Transform_SetScale(Entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public bool IsDirty => Internal.Transform_IsDirty(Entity.ID);

		public Vector3 Forward => Internal.Transform_GetForward(Entity.ID);
		public Vector3 Up => Internal.Transform_GetUp(Entity.ID);
		public Vector3 Right => Internal.Transform_GetRight(Entity.ID);
	}
	// -------------------
}
