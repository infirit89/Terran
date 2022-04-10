
namespace TerranScriptCore
{
	public class Component
	{
		public Entity entity;
	}

	public class Rigidbody2D : Component 
	{
		public bool FixedRotation 
		{
			get => Internal.Rigidbody2D_IsFixedRotation_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D_SetFixedRotation_Internal(entity.ID.Data, value);
		}

		public RigidbodySleepState AwakeState 
		{
			get => (RigidbodySleepState)Internal.Rigidbody2D_GetAwakeState_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D_SetAwakeState_Internal(entity.ID.Data, (byte)value);
		}

		public float GravityScale
		{
			get => Internal.Rigidbody2D_GetGravityScale_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D_SetGravityScale_Internal(entity.ID.Data, value);
		}

		public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForce_Internal(entity.ID.Data, in force, in position, (byte)forceMode);

		public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForceAtCenter_Internal(entity.ID.Data, in force, (byte)forceMode);
	}

	public abstract class Collider2D : Component 
	{
		public Vector2 Offset 
		{
			get => GetOffset();
			set => SetOffset(value);
		}

		public bool IsSensor 
		{
			get => IsSensorF();
			set => SetSensor(value);
		}

		protected abstract Vector2 GetOffset();
		protected abstract void SetOffset(Vector2 offset);

		protected abstract bool IsSensorF();
		protected abstract void SetSensor(bool isSensor);
	}

	public class BoxCollider2D : Collider2D 
	{
		public Vector2 Size 
		{
			get 
			{
				Vector2 size;
				Internal.BoxCollider2D_GetSize_Internal(entity.ID.Data, out size);
				return size;
			}

			set => Internal.BoxCollider2D_SetSize_Internal(entity.ID.Data, in value);
		}

		protected override Vector2 GetOffset() 
		{
			Vector2 offset;
			Internal.BoxCollider2D_GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
		}

		protected override void SetOffset(Vector2 offset) => Internal.BoxCollider2D_SetOffset_Internal(entity.ID.Data, in offset);

		protected override bool IsSensorF() => Internal.BoxCollider2D_IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => Internal.BoxCollider2D_SetSensor_Internal(entity.ID.Data, isSensor);
    }

    public class CircleCollider2D : Collider2D
    {
		public float Radius 
		{
			get => Internal.CircleCollider2D_GetRadius_Internal(entity.ID.Data);
			set => Internal.CircleCollider2D_SetRadius_Internal(entity.ID.Data, value);
		}

        protected override Vector2 GetOffset()
        {
			Vector2 offset;
			 Internal.CircleCollider2D_GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
        }

		protected override void SetOffset(Vector2 offset) =>  Internal.CircleCollider2D_SetOffset_Internal(entity.ID.Data, in offset);

		protected override bool IsSensorF() => Internal.CircleCollider2D_IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => Internal.CircleCollider2D_SetSensor_Internal(entity.ID.Data, isSensor);
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
					return Internal.Tag_GetName_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(entity != null)
					Internal.Tag_SetName_Internal(entity.ID.Data, value);

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
					Vector3 outVec =  Internal.Transform_GetPosition_Internal(entity.ID.Data);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetPosition_Internal(entity.ID.Data, in value);
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetRotation_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetRotation_Internal(entity.ID.Data, in value);

				// TODO: log that the entity is null
			}
		}

		public Vector3 Scale
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetScale_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetScale_Internal(entity.ID.Data, in value);

				// TODO: log that the entity is null
			}
		}

		bool IsDirty => Internal.Transform_IsDirty_Internal(entity.ID.Data);

	}
	// -------------------
}
