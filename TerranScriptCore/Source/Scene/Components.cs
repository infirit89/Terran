
namespace Terran
{
	public class Component
	{
		public Entity entity;
	}

	public class Rigidbody2D : Component 
	{
		public bool FixedRotation 
		{
			get => Internal.Rigidbody2D_IsFixedRotation(entity.ID);
			set => Internal.Rigidbody2D_SetFixedRotation(entity.ID, value);
		}

		public RigidbodySleepState AwakeState 
		{
			get => (RigidbodySleepState)Internal.Rigidbody2D_GetSleepState(entity.ID);
			set => Internal.Rigidbody2D_SetSleepState(entity.ID, (byte)value);
		}

		public float GravityScale
		{
			get => Internal.Rigidbody2D_GetGravityScale(entity.ID);
			set => Internal.Rigidbody2D_SetGravityScale(entity.ID, value);
		}
		
		public RigidbodyType BodyType 
		{
			get => (RigidbodyType)Internal.Rigidbody2D_GetType(entity.ID);
			set => Internal.Rigidbody2D_SetType(entity.ID, (byte)value);
		}

		public Vector2 LinearVelocity 
		{
			get
			{
				Vector2 linearVelocity;
				Internal.Rigidbody2D_GetLinearVelocity(entity.ID, out linearVelocity);
				return linearVelocity;
			}

			set => Internal.Rigidbody2D_SetLinearVelocity(entity.ID, in value);
		}

		public float AngularVelocity 
		{
			get => Internal.Rigidbody2D_GetAngularVelocity(entity.ID);
			set => Internal.Rigidbody2D_SetAngularVelocity(entity.ID, value);
		}

		public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForce(entity.ID, in force, in position, (byte)forceMode);

		public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForceAtCenter(entity.ID, in force, (byte)forceMode);
	}

	public class Collider2D : Component
	{
		protected enum ColliderType : byte
		{
			None = 0,
			Box,
			Circle
		}
		public Collider2D() { }

		protected Collider2D(ColliderType type) 
		{
			p_ColliderType = type;
		}

		public Vector2 Offset 
		{
			get 
			{
				Vector2 offset;
				Internal.Collider2D_GetOffset(entity.ID, (byte)p_ColliderType, out offset);
				return offset;
			}
			set => Internal.Collider2D_SetOffset(entity.ID, (byte)p_ColliderType, in value);
		}

		public bool IsSensor 
		{
			get => Internal.Collider2D_IsSensor(entity.ID, (byte)p_ColliderType);
			set => Internal.Collider2D_SetSensor(entity.ID, (byte)p_ColliderType, value);
		}

		protected ColliderType p_ColliderType = ColliderType.None;
	}

	public class BoxCollider2D : Collider2D 
	{
		public BoxCollider2D() : base(ColliderType.Box) { }

		public Vector2 Size 
		{
			get 
			{
				Vector2 size;
				Internal.BoxCollider2D_GetSize(entity.ID, out size);
				return size;
			}

			set => Internal.BoxCollider2D_SetSize(entity.ID, in value);
		}
    }

    public class CircleCollider2D : Collider2D
    {
		public CircleCollider2D() : base(ColliderType.Circle) { }

		public float Radius 
		{
			get => Internal.CircleCollider2D_GetRadius(entity.ID);
			set => Internal.CircleCollider2D_SetRadius(entity.ID, value);
		}
	}

	// ---- Scriptable ----
	public class Scriptable : Component
	{
		public Scriptable() { }

		internal Scriptable(byte[] id) 
		{
			if (entity == null) 
				entity = new Entity(id);
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
				if (entity != null) 
					return Internal.Tag_GetName(entity.ID);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(entity != null)
					Internal.Tag_SetName(entity.ID, value);

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
				if (entity != null) 
				{
					Vector3 outVec =  Internal.Transform_GetPosition(entity.ID);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetPosition(entity.ID, in value);
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetRotation(entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetRotation(entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public Vector3 Scale
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetScale(entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetScale(entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public bool IsDirty => Internal.Transform_IsDirty(entity.ID);

		public Vector3 Forward => Internal.Transform_GetForward(entity.ID);
		public Vector3 Up => Internal.Transform_GetUp(entity.ID);
		public Vector3 Right => Internal.Transform_GetRight(entity.ID);
	}
	// -------------------
}
