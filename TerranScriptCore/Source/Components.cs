
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
			get => Internal.Rigidbody2D_IsFixedRotation_Internal(entity.ID);
			set => Internal.Rigidbody2D_SetFixedRotation_Internal(entity.ID, value);
		}

		public RigidbodySleepState AwakeState 
		{
			get => (RigidbodySleepState)Internal.Rigidbody2D_GetAwakeState_Internal(entity.ID);
			set => Internal.Rigidbody2D_SetAwakeState_Internal(entity.ID, (byte)value);
		}

		public float GravityScale
		{
			get => Internal.Rigidbody2D_GetGravityScale_Internal(entity.ID);
			set => Internal.Rigidbody2D_SetGravityScale_Internal(entity.ID, value);
		}

		public Vector2 LinearVelocity 
		{
			get
			{
				Vector2 linearVelocity;
				Internal.Rigidbody2D_GetLinearVelocity_Internal(entity.ID, out linearVelocity);
				return linearVelocity;
			}

			set => Internal.Rigidbody2D_SetLinearVelocity_Internal(entity.ID, in value);
		}

		public float AngularVelocity 
		{
			get => Internal.Rigidbody2D_GetAngularVelocity_Internal(entity.ID);
			set => Internal.Rigidbody2D_SetAngularVelocity_Internal(entity.ID, value);
		}

		public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForce_Internal(entity.ID, in force, in position, (byte)forceMode);

		public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode) => Internal.Rigidbody2D_ApplyForceAtCenter_Internal(entity.ID, in force, (byte)forceMode);
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
				Internal.BoxCollider2D_GetSize_Internal(entity.ID, out size);
				return size;
			}

			set => Internal.BoxCollider2D_SetSize_Internal(entity.ID, in value);
		}

		protected override Vector2 GetOffset() 
		{
			Vector2 offset;
			Internal.BoxCollider2D_GetOffset_Internal(entity.ID, out offset);
			return offset;
		}

		protected override void SetOffset(Vector2 offset) => Internal.BoxCollider2D_SetOffset_Internal(entity.ID, in offset);

		protected override bool IsSensorF() => Internal.BoxCollider2D_IsSensor_Internal(entity.ID);

		protected override void SetSensor(bool isSensor) => Internal.BoxCollider2D_SetSensor_Internal(entity.ID, isSensor);
    }

    public class CircleCollider2D : Collider2D
    {
		public float Radius 
		{
			get => Internal.CircleCollider2D_GetRadius_Internal(entity.ID);
			set => Internal.CircleCollider2D_SetRadius_Internal(entity.ID, value);
		}

        protected override Vector2 GetOffset()
        {
			Vector2 offset;
			 Internal.CircleCollider2D_GetOffset_Internal(entity.ID, out offset);
			return offset;
        }

		protected override void SetOffset(Vector2 offset) =>  Internal.CircleCollider2D_SetOffset_Internal(entity.ID, in offset);

		protected override bool IsSensorF() => Internal.CircleCollider2D_IsSensor_Internal(entity.ID);

		protected override void SetSensor(bool isSensor) => Internal.CircleCollider2D_SetSensor_Internal(entity.ID, isSensor);
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
					return Internal.Tag_GetName_Internal(entity.ID);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(entity != null)
					Internal.Tag_SetName_Internal(entity.ID, value);

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
					Vector3 outVec =  Internal.Transform_GetPosition_Internal(entity.ID);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetPosition_Internal(entity.ID, in value);
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetRotation_Internal(entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetRotation_Internal(entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public Vector3 Scale
		{
			get
			{
				if (entity != null) 
					return Internal.Transform_GetScale_Internal(entity.ID);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform_SetScale_Internal(entity.ID, in value);

				// TODO: log that the entity is null
			}
		}

		public bool IsDirty => Internal.Transform_IsDirty_Internal(entity.ID);

		public Vector3 Forward => Internal.Transform_GetForward_Internal(entity.ID);
		public Vector3 Up => Internal.Transform_GetUp_Internal(entity.ID);
		public Vector3 Right => Internal.Transform_GetRight_Internal(entity.ID);
	}
	// -------------------
}
