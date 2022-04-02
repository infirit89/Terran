using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
	public class Component
	{
		public Entity entity;
	}

	public enum Rigidbody2DAwakeState : byte
	{
		Sleep = 0,
		Awake,
		NeverSleep
	}

	public class Rigidbody2D : Component 
	{
		public bool FixedRotation 
		{
			get => IsFixedRotation_Internal(entity.ID.Data);
			set => SetFixedRotation_Internal(entity.ID.Data, value);
		}

		public Rigidbody2DAwakeState AwakeState 
		{
			get => (Rigidbody2DAwakeState)GetAwakeState_Internal(entity.ID.Data);
			set => SetAwakeState_Internal(entity.ID.Data, (byte)value);
		}

		public float GravityScale
		{
			get => GetGravityScale_Internal(entity.ID.Data);
			set => SetGravityScale_Internal(entity.ID.Data, value);
		}

		public void ApplyForce(Vector2 force, Vector2 position) => ApplyForce_Internal(entity.ID.Data, force, position);

		public void ApplyForceAtCenter(Vector2 force) => ApplyForceAtCenter_Internal(entity.ID.Data, force);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsFixedRotation_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetFixedRotation_Internal(byte[] entityUUID, bool fixedRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern byte GetAwakeState_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetAwakeState_Internal(byte[] entityUUID, byte awakeState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern float GetGravityScale_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetGravityScale_Internal(byte[] entityUUID, float gravityScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void ApplyForce_Internal(byte[] entityUUID, Vector2 force, Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void ApplyForceAtCenter_Internal(byte[] entityUUID, Vector2 force);
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
				GetSize_Internal(entity.ID.Data, out size);
				return size;
			}

			set => SetSize_Internal(entity.ID.Data, value);
		}

		protected override Vector2 GetOffset() 
		{
			Vector2 offset;
			GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
		}

		protected override void SetOffset(Vector2 offset) => SetOffset_Internal(entity.ID.Data, offset);

		protected override bool IsSensorF() => IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => SetSensor_Internal(entity.ID.Data, isSensor);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void GetSize_Internal(byte[] entityID, out Vector2 outSize);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetSize_Internal(byte[] entityID, Vector2 size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void GetOffset_Internal(byte[] entityID, out Vector2 outOffset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetOffset_Internal(byte[] entityUUID, Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsSensor_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetSensor_Internal(byte[] entityUUID, bool isSensor);
    }

    public class CircleCollider2D : Collider2D
    {
		public float Radius 
		{
			get => GetRadius_Internal(entity.ID.Data);
			set => SetRadius_Internal(entity.ID.Data, value);
		}

        protected override Vector2 GetOffset()
        {
			Vector2 offset;
			GetOffset_Internal(entity.ID.Data, out offset);
			return offset;
        }

		protected override void SetOffset(Vector2 offset) => SetOffset_Internal(entity.ID.Data, offset);

		protected override bool IsSensorF() => IsSensor_Internal(entity.ID.Data);

		protected override void SetSensor(bool isSensor) => SetSensor_Internal(entity.ID.Data, isSensor);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern float GetRadius_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetRadius_Internal(byte[] entityID, float radius);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void GetOffset_Internal(byte[] entityID, out Vector2 outVec);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetOffset_Internal(byte[] entityUUID, Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsSensor_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetSensor_Internal(byte[] entityUUID, bool isSensor);
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
					return GetTagName_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return "";
			}

			set
			{
				if(entity != null)
					SetTagName_Internal(entity.ID.Data, value);

				// TODO: log that the entity is null
			}
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetTagName_Internal(byte[] entityID, in string inName);
		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern string GetTagName_Internal(byte[] entityID);
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
					Vector3 outVec = GetTransformPosition_Internal(entity.ID.Data);
					return outVec;
				}

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
				{
					SetTransformPosition_Internal(entity.ID.Data, value);
				}
				// TODO: log that the entity is null
			}
		}

		public Vector3 Rotation
		{
			get
			{
				if (entity != null) 
					return GetTransformRotation_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
				{
					SetTransformRotation_Internal(entity.ID.Data, value);
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
					return GetTransformScale_Internal(entity.ID.Data);

				// TODO: log that the entity is null
				return new Vector3(0.0f, 0.0f, 0.0f);
			}

			set
			{
				if (entity != null) 
				{
					SetTransformScale_Internal(entity.ID.Data, value);
					return;
				}

				// TODO: log that the entity is null
			}
		}

		bool IsDirty 
		{
			get => IsDirty_Internal(entity.ID.Data);
		}

		// ---- position ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern Vector3 GetTransformPosition_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetTransformPosition_Internal(byte[] entityID, Vector3 inPosition);
		// ------------------

		// ---- rotation ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern Vector3 GetTransformRotation_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetTransformRotation_Internal(byte[] entityID, Vector3 inRotation);
		// ------------------

		// ---- scale ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern Vector3 GetTransformScale_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void SetTransformScale_Internal(byte[] entityID, Vector3 inScale);
		// ---------------

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsDirty_Internal(byte[] entityID);
	}
	// -------------------
}
