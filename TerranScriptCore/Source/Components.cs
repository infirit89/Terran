
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
			get => Internal.Rigidbody2D.IsFixedRotation_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D.SetFixedRotation_Internal(entity.ID.Data, value);
		}

		public RigidbodySleepState AwakeState 
		{
			get => (RigidbodySleepState)Internal.Rigidbody2D.GetAwakeState_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D.SetAwakeState_Internal(entity.ID.Data, (byte)value);
		}

		public float GravityScale
		{
			get => Internal.Rigidbody2D.GetGravityScale_Internal(entity.ID.Data);
			set => Internal.Rigidbody2D.SetGravityScale_Internal(entity.ID.Data, value);
		}

		public void ApplyForce(Vector2 force, Vector2 position, ForceMode2D forceMode) => Internal.Rigidbody2D.ApplyForce_Internal(entity.ID.Data, in force, in position, (byte)forceMode);

		public void ApplyForceAtCenter(Vector2 force, ForceMode2D forceMode) => Internal.Rigidbody2D.ApplyForceAtCenter_Internal(entity.ID.Data, in force, (byte)forceMode);
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
				Internal.BoxCollider2D.GetSize_Internal(entity.ID.Data, out size);
				return size;
			}

			set => Internal.BoxCollider2D.SetSize_Internal(entity.ID.Data, in value);
		}

		protected override Vector2 GetOffset() 
		{
			Vector2 offset;
			Internal.BoxCollider2D.GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
		}

		protected override void SetOffset(Vector2 offset) => Internal.BoxCollider2D.SetOffset_Internal(entity.ID.Data, in offset);

		protected override bool IsSensorF() => Internal.BoxCollider2D.IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => Internal.BoxCollider2D.SetSensor_Internal(entity.ID.Data, isSensor);
    }

    public class CircleCollider2D : Collider2D
    {
		public float Radius 
		{
			get => Internal.CircleCollider2D.GetRadius_Internal(entity.ID.Data);
			set => Internal.CircleCollider2D.SetRadius_Internal(entity.ID.Data, value);
		}

        protected override Vector2 GetOffset()
        {
			Vector2 offset;
			 Internal.CircleCollider2D.GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
        }

		protected override void SetOffset(Vector2 offset) =>  Internal.CircleCollider2D.SetOffset_Internal(entity.ID.Data, in offset);

		protected override bool IsSensorF() => Internal.CircleCollider2D.IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => Internal.CircleCollider2D.SetSensor_Internal(entity.ID.Data, isSensor);
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
					return Internal.Tag.GetName_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(entity != null)
					Internal.Tag.SetName_Internal(entity.ID.Data, value);

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
					Vector3 outVec =  Internal.Transform.GetPosition_Internal(entity.ID.Data);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform.SetPosition_Internal(entity.ID.Data, in value);
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (entity != null) 
					return Internal.Transform.GetRotation_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform.SetRotation_Internal(entity.ID.Data, in value);

				// TODO: log that the entity is null
			}
		}

		public Vector3 Scale
		{
			get
			{
				if (entity != null) 
					return Internal.Transform.GetScale_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
					Internal.Transform.SetScale_Internal(entity.ID.Data, in value);

				// TODO: log that the entity is null
			}
		}

		bool IsDirty => Internal.Transform.IsDirty_Internal(entity.ID.Data);

	}
	// -------------------
}
